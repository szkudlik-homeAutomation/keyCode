#pragma once


#define FW_VERSION_MAJOR 2
#define FW_VERSION_MINOR 0
#define FW_VERSION_PATCH 2


#define stringify_literal( x ) # x
#define stringify_expanded( x ) stringify_literal( x )

#define CONCATENATE_FW_VERSION(A,B,C) stringify_expanded(A) "." stringify_expanded(B) "." stringify_expanded(C)

#define FW_VERSION CONCATENATE_FW_VERSION(FW_VERSION_MAJOR,FW_VERSION_MINOR,FW_VERSION_PATCH)
