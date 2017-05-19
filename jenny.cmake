macro(j_prepare)
# This file would be called multiple times by jenny
# jenny will call this as cmake script first to get the customized configurations, with a read only variable j_is_prepare 

# Read only variable. Path to project config file, default is jenny.cmake in the root of project source
# message(Procession ${j_project_file})

# set project name, must be a valid file name without space.
# default is empty. 
# if it's empty, jenny will use the parent dir name of j_project_file
set(j_project_name "cserver")

# set the root dir of project source
# default is empty, means the dir of j_project_file located
#set(j_project_root "")

# root dir of project output
# default is empty, and means ${j_project_root}/../Toolkit
#set(j_output_root_dir "")

# the dir to put generated projects. 
# default is empty and means ${j_output_root_dir}/${j_project_name}
#set(j_project_output_root_dir "")

# root dir to put dependent projects.
# default is empty and means the parent dir of j_project_root
#set(j_sibling_root_dir "")

# dir name for each config
# default is empty, it means "debug" for Debug config, and "release" for Release config
#set(j_config_dir "")

# dir to put generate projects.
# default is empty, means ${j_project_output_root_dir}/build/${j_platform}/${j_toolset}_${j_toolset_ver}/${j_architecture}/${j_config_dir} 
#set(j_generated_project_dir "")

# root dir of all CMakeLists.txt
# default is "build/projects"
# set(j_projects_dir "build/projects")

# Read only, passed in by command line. can be xcode, gcc, msvc etc
#message(${j_toolset})

# Read only, passed in by command line. version of toolset.
# for xcode, it could be 7, 8.1 etc.
# for msvc, it would be 140
# for gcc, it would be 4, 4.9, or 5 (TODO: not ready yet)
# message(${j_toolset_ver})

# Read only, passedin by command line. project target platform
# can be win, mac, ios, linux etc.
#message(j_platform)

# Read only, passedin by command line. C++ compiler
# can be clang, gcc, cl etc
#message(j_compiler)

# Read only, passedin by command line. compiler version.
# format is major.minor
#message(j_compiler_version)

# Read only, passedin by command line. project target architecture.
# can be x64, x86, armel, armhf etc.
#message(j_architecture)

# set http user & password for downloading 3rd party
# set(j_server_user_passwd "")
endmacro()	# end macro j_prepare

# this would be called only once, and before loading any CMakeLists.
macro (j_main)
	set(j_config_postfix "")
	set(j_verbose ON)

endmacro()

function(j_pre_sync_external_projects)
endfunction()

function(j_post_sync_external_projects)
endfunction()

function(j_pre_sync_libraries)
endfunction()

function(j_post_sync_libraries)
endfunction()

function(j_post_add_projects)
endfunction()


