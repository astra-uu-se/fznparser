# FlatZinc parser
This is a C++ parser that parses and synthesises FlatZinc (.fzn) model files.

# Include
```cmake
include(FetchContent)

FetchContent_Declare(
  fznparser
  GIT_REPOSITORY git@github.com:astra-uu-se/fznparser.git
  GIT_TAG 1.0.0)

FetchContent_MakeAvailable(fznparser)

target_link_libraries(... fznparser::fznparser ...)
```

# Building & Testing
Building: 

```
$ make build
``` 

Building and running the tests:

```
$ make test
``` 

If you have MiniZinc, then you can retrieve and flatten each MiniZinc challange instance via:

```sh
python3 test/flatten.py
```

Running the tests will now also parse each flattened MiniZinc instance.