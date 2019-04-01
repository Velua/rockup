cmake_minimum_required(VERSION 3.5)
project(rockup VERSION 1.0.0)

find_package(eosio.cdt)

# add contract
add_contract( rockup rockup rockup.cpp )

### add the path to where the ricardian contracts/clauses are found
target_compile_options( rockup.wasm PUBLIC -R${CMAKE_CURRENT_SOURCE_DIR} )
