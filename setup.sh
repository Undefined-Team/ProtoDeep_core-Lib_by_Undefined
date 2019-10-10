#!/bin/bash

# ------------------------------------------------------------- #

# How to use setup.sh ?
# 1 chmod +x setup.sh
# 2 ./setup.sh {parameter1} {parameter2}...

# Parameter list
# -> fclean (make fclean in current project folder before make)
# -> libclean (remove main lib folder, then all dependencies)
# -> noupdate (Will not check if update is needed recursively)
# -> nodepmake (Will not make dependencies and sub dependencies)

# ------------------------------------------------------------- #


# 0 - Utils
BLUE='\033[1;34m'
GREEN='\033[1;32m'
RED='\033[1;31m'
NC='\033[0m'

function error_print {
    printf "$RED" >&2
    printf "$2Error: $1$NC\n\n" >&2
    exit 1
}

function success_print {
    printf "$GREEN" >&2
    printf "$2Success: $1$NC\n" >&2
}

function info_print {
    printf "$BLUE$2$1$NC\n" >&2
}

function space_trim {
    local var="$*"
    var="${var#"${var%%[![:space:]]*}"}"
    var="${var%"${var##*[![:space:]]}"}"   
    echo -n "$var"
}

function csv_param_trim {
    local var="$*"
    var=${var//[$'\t\r\n"']}
    var=$(space_trim "$var")
    var=$(echo "$var" | sed -r 's/[ ]+/_/g')
    echo -n "$var"
}

function basic_trim {
    local var="$*"
    var=$(echo "$var" | tr '\n' ' ')
    var=$(echo "$var" | tr '\t' ' ')
    var=$(echo "$var" | tr '\r' ' ')
    var=$(space_trim "$var")
    echo -n "$var"
}

function is_error {
    [[ "$*" != "0" ]]
}

function get_name_in_dep_tree {
    local trimed
    eval "local toread=$'$1'"
    while IFS=$'\n' read -r line; do
        IFS=" " read -a fields <<<"$line"
        trimed=$(basic_trim "${fields[0]}")
        if [[ "$trimed" == "$2" ]] ; then
            echo -n "$line"
            return
        fi
    done <<< "$toread"
    echo -n "1"
}

function is_in_header {
    IFS=" " read -a dep_header_f <<< "$1"
    for (( j = ${#ret_f[@]} - 1; j >= 0; --j )); do
        if [[ "${dep_header_f[j]}" == "$2" ]] ; then
            true
            return
        fi
    done
    false
}

function dep_header_add {
    local dep_header="$1"
    local trimed
    IFS=" " read -a ret_f <<< "$2"
    for (( i = ${#ret_f[@]} - 1; i >= 0; --i )); do
        trimed=$(basic_trim "${ret_f[i]}")
        if ! is_in_header "$dep_header" "$trimed" ; then
            dep_header="$dep_header $trimed"
        fi
    done
    echo -n "$dep_header"
}

function dep_header_format {
    local dep_header=""
    IFS=" " read -a dep_header_f <<< "$1"
    for (( i = ${#dep_header_f[@]} - 1; i >= 0; --i )); do
        trimed=$(basic_trim "${dep_header_f[i]}")
        dep_header="$2/include/ud_${dep_header_f[i]}.h $dep_header"
    done
    echo -n "$dep_header"
}

if [[ $1 == "help" ]] ; then
    info_print "\n How to use setup.sh ?"
    printf "\t$GREEN 1 chmod +x setup.sh$NC\n"
    printf "\t$GREEN 2 ./setup.sh {parameter1} {parameter2}...$NC\n"
    info_print "\n Parameter list"
    printf "\t$GREEN -> help (Will show you how to use setup.sh)$NC\n"
    printf "\t$GREEN -> fclean (make fclean in current project folder before make)$NC\n"
    printf "\t$GREEN -> libclean (remove main lib folder, then all dependencies)$NC\n"
    printf "\t$GREEN -> noupdate (Will not check if update is needed recursively)$NC\n"
    printf "\t$GREEN -> nodepmake (Will not make dependencies and sub dependencies)$NC\n\n"
    exit 0
else
    ! $dep_recursive && { info_print "\n Use \"./setup.sh help\" to see parameters"; }
fi

has_set_env=false
noupdate=false
nodepmake=false
dep_tree=""

function start_recursive {
    local location
    local dep_recursive
    if [ ! -z "$1" ] && [ $1 != "dep_recursive" ] || [ -z "$1" ] ; then
        dep_recursive=false
        location="$( cd "$( dirname "${BASH_SOURCE[0]}" )" > /dev/null 2>&1 && pwd )"
    else
        dep_recursive=true
        location="$2"
    fi
    conf_path="$location/conf"

    # 1 - Get configuration
    ! $dep_recursive && { info_print "\n (1) Get configuration in $conf_path/conf.csv"; }
    local i=0
    local col1
    local col2
    local target_name
    local ud_lib_path
    [ ! -f "$conf_path"/conf.csv ] && { error_print "Configuration file not found" "\t"; }
    while IFS=, read -r col1 col2
    do
        col1=$(csv_param_trim "$col1")
        col2=$(csv_param_trim "$col2")
        # Get name and main lib folder path of the project
        target_name="$col1"
        eval "local ud_lib_path=\"$col2\""
        i=1
        ! $dep_recursive && { success_print "Set target lib name to  [ $col1 ] and main lib path to [ $ud_lib_path ]" "\t"; }
    done < "$conf_path"/conf.csv
    ! $dep_recursive && { success_print "All done" "\t"; }

    # 2 - Preprocessing
    ! $dep_recursive && { info_print "\n (2) Preprocessing"; }
    for pparam in "$@"
    do
        # If fclean parameter detected, fclean project
        if [[ $pparam == "fclean" ]] ; then
            make -C "$location" fclean > /dev/null 2>&1
            is_error $? && { error_print "Can't make fclean in $location folder" "\t"; }
            success_print "Make fclean in [ $location ] folder" "\t"
        # If libclean parameter detected, remove main lib folder (Full reset)
        elif [[ $pparam == "libclean" ]] ; then
            !(rm -rf "$ud_lib_path") && { error_print "Can't remove main lib folder" "\t"; }
            success_print "Main lib folder removed" "\t"
        elif [[ $pparam == "noupdate" ]] ; then
            noupdate=true
        elif [[ $pparam == "nodepmake" ]] ; then
            nodepmake=true
        fi
    done
    ! $dep_recursive && { success_print "All done" "\t"; }

    # 3 - Check update
    ! $dep_recursive && { info_print "\n (3) Check if need update"; }
    if ! $noupdate ; then
        gitret=$(git -C "$location" pull 2>&1) > /dev/null
        is_error $? && { error_print "Can't git pull" "\t"; }
        if [[ "$gitret" != "Already up to date." ]] && ! is_error $? ; then
            $dep_recursive && { info_print "[ $target_name ] need to be updated" "\t"; }
            success_print "Files updated" "\t"
        fi
    else
        ! $dep_recursive && { success_print "No update parameter detected" "\t"; }
    fi
    ! $dep_recursive && { success_print "All done" "\t"; }

    # 4 - Get dependencies
    ! $dep_recursive && { info_print "\n (4) Get dependencies in $conf_path/dependencies.csv"; }
    local dependencies=()
    i=0
    [ ! -f "$conf_path"/dependencies.csv ] && { error_print "Dependencies file not found" "\t"; }
    while IFS=, read -r col1 col2
    do
        col1=$(csv_param_trim "$col1")
        col2=$(csv_param_trim "$col2")
        # Get dependencies
        dependencies+=("link='$col1' && name='$col2'")
        ! $dep_recursive && { success_print "Found dependency [ $col2 ] with git link [ $col1 ]" "\t"; }
    done < "$conf_path"/dependencies.csv
    ! $dep_recursive && { success_print "All done" "\t"; }

    if ! $dep_recursive ; then
        # 5 - Set up path in env var
        info_print "\n (5) Check if need create main lib env path";
        new_path_array=("$ud_lib_path/lib" "$ud_lib_path/include")
        path_name_array=("LIBRARY_PATH" "C_INCLUDE_PATH")
        path_array=("$LIBRARY_PATH" "$C_INCLUDE_PATH")
        path_var_array=(LIBRARY_PATH C_INCLUDE_PATH)
        bashrc_path="$HOME/.bashrc"
        for i in "${!new_path_array[@]}"; do 
            # Set env var in current shell
            if [[ ! $(env) == *"${new_path_array[$i]}"* ]]; then
                has_set_env=true
                cmd="export ${path_var_array[$i]}='${new_path_array[$i]}:${path_array[$i]}'"
                eval $cmd
                if [[ ! $(sh -c env) == *"${new_path_array[$i]}"* ]]; then
                    error_print "Can't add [ ${new_path_array[$i]} ] in [ ${path_name_array[$i]} ]" "\t"
                fi
                success_print "Env var [ ${path_name_array[$i]} ] now contains [ ${new_path_array[$i]} ]" "\t"
            fi
            # Set env var in bashrc
            if [[ ! $(cat $bashrc_path) == *"${new_path_array[$i]}"* ]]; then
                has_set_env=true
                echo "export ${path_name_array[$i]}='${new_path_array[$i]}:${path_array[$i]}'" >> $bashrc_path
                if [[ ! $(cat $bashrc_path) == *"${new_path_array[$i]}"* ]]; then
                    error_print "Can't add export command of [ ${new_path_array[$i]} ] in [ ${path_name_array[$i]} ] in bashrc file" "\t"
                fi
            fi
        done
        success_print "All done" "\t"

        # 6 - Create folder
        info_print "\n (6) Check if need create main lib folder" 
        lib_folder_array=("${ud_lib_path}" "${ud_lib_path}/lib" "${ud_lib_path}/include" "${ud_lib_path}/clone")
        for lib_folder in "${lib_folder_array[@]}"; do
            if [ ! -d "$lib_folder" ]; then
                mkdir -p "$lib_folder"
                is_error $? && { error_print "Can't create  [ $lib_folder ] folder" "\t"; }
                success_print "[ $lib_folder ] folder created" "\t"
            fi
        done
        success_print "All done" "\t"
    fi

    # 7 - Dependencies
    ! $dep_recursive && { info_print "\n (7) Check if need install/update dependencies"; }
    local link
    local name
    local make_dep_name=""
    local make_ar_name=""
    local actual_folder
    local ret
    local dep_lst=""
    local dep_header="$target_name"
    for dep in "${dependencies[@]}"; do
        eval "$dep"
        actual_folder="${ud_lib_path}/clone/$name"
        ret=$(get_name_in_dep_tree "$dep_tree" $name)
        # If dependency already visited
        if [[ "$ret" == "1" ]] ; then
            # Check if dependency need to be installed
            if [ ! -d "$actual_folder" ]; then
                # Download dependency
                info_print "[ $name ] dependency need to be installed" "\t"
                git clone "$link" "$actual_folder" > /dev/null 2>&1
                is_error $? && { error_print "Can't download dependency [ $name ] <-> [ $link ]" "\t"; }
                success_print "Dependency was downloaded" "\t"
                # Chmod dependency
                chmod +x "$actual_folder/setup.sh" > /dev/null 2>&1
                is_error $? && { error_print "Can't chmod dependency" "\t"; }
                success_print "Dependency was chmoded" "\t"
                # Install dependency
                success_print "Dependency is installing..." "\t"
            fi
            ret=$(start_recursive "dep_recursive" "$actual_folder")
            is_error $? && { error_print "Can't scan dependency [ $name ] <-> [ $link ]" "\t"; }
            dep_tree="$dep_tree$ret\n"
        fi
        dep_header=$(dep_header_add "$dep_header" "$ret")
        dep_lst="$name $dep_lst"
        make_dep_name="$make_dep_name -lud_${name//'"'}"
        make_ar_name="$make_ar_name $ud_lib_path/lib/libud_${name//'"'}.a"
    done
    dep_header=$(dep_header_format "$dep_header" "$ud_lib_path")
    ! $dep_recursive && { success_print "All done" "\t"; }

    # 8 - Install
    if ! $dep_recursive || ! $nodepmake ; then
        ! $dep_recursive && { info_print "\n (8) Start compiling"; }
        # Copy headers in main lib folder
        if [ ! -f "$ud_lib_path"/include/ud_"$target_name".h ] || [[ $(diff "$location"/res/include/ud_"$target_name".h "$ud_lib_path"/include/ud_"$target_name".h) != "" ]] ; then
            cp "$location"/res/include/* "$ud_lib_path"/include/
            is_error $? && { error_print "Copy headers files to [ $ud_lib_path/include/ ] failed"; }
        fi
        # Compil
        if ! $dep_recursive ; then
            make -C "$location" --no-print- LIBNAME="$target_name" DEPNAME="$make_dep_name" ARNAME="$make_ar_name" DEPHEADER="$dep_header" >&2
        else
            make -C "$location" --no-print- LIBNAME="$target_name" DEPNAME="$make_dep_name" ARNAME="$make_ar_name" DEPHEADER="$dep_header" > /dev/null 2>&1
        fi
        is_error $? && { error_print "Compilation failed"; }
        # Copy lib in main lib folder
        cp "$location"/*.a "$ud_lib_path"/lib/
        is_error $? && { error_print "Copy compiled files to [ $ud_lib_path/lib/ ] failed"; }
        if ! $dep_recursive ; then
            success_print "All done\n" "\t"
            success_print "Install completed."
            if $has_set_env ; then
                success_print "Shell restarting...\n"
                exec $SHELL
            fi
            printf "\n"
        fi
    fi
    $dep_recursive && { echo "$target_name $dep_lst"; }
}

start_recursive $@