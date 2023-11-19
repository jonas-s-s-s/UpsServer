#Includes cmake module which allows us to use the AddTests(<TARGET_NAME>) function
include(Testing)

add_subdirectory(server)
add_subdirectory(parser)
add_subdirectory(game)