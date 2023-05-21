# Utils

Is the base library used for most of the projects here.

It provides data structures and declarations in a common namespace.

## Testing

The Test directory is setup to work with [googletest](https://github.com/google/googletest).

## Building

![A1](https://github.com/chcly/Utils/actions/workflows/build-linux.yml/badge.svg)
![A2](https://github.com/chcly/Utils/actions/workflows/build-windows.yml/badge.svg)

```sh
mkdir build
cd build
cmake .. -DUtils_BUILD_TEST=ON -DUtils_AUTO_RUN_TEST=ON
make
```

### Optional defines

| Option                   | Description                                          | Default |
| :----------------------- | :--------------------------------------------------- | :-----: |
| Utils_BUILD_TEST         | Build the unit test program.                         |   ON    |
| Utils_AUTO_RUN_TEST      | Automatically run the test program.                  |   OFF   |
| Utils_USE_STATIC_RUNTIME | Build with the MultiThreaded(Debug) runtime library. |   ON    |
