# Core Types Functional Cluster 

Core Types defines common classes and functionality used by multiple Functional Clusters as part of their public interfaces.

## Building project

```sh
mkdir build && cd build
meson setup -Db_coverage=true --pkg-config-path .
ninja
```

### Running tests and collecting test coverage

```sh
Run tests and coverage:
ninja test
ninja coverage-html
```