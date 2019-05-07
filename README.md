# torhash

This is a simple (and **insecure**) implementation of the salted hash used by Tor's control port authentication for the "hashed password" method.

## Build

The only requirement is the [LibTomCrypt](https://www.libtom.net/LibTomCrypt/) library. [CMake](https://cmake.org) is used for generating build files, on Windows, you will also need [MSYS2](https://www.msys2.org/) which should come with the MinGW-w64 `gcc` compiler, don't forget to choose the desired shell before compiling.

After making sure the requirements are installed, you can build running a relatively standard set of commands in the project directory:
```
$ cmake .
$ make
```

## Usage

It is very simple to use `torhash`! It interprets all arguments as strings and computes the hash for each of them. The final hashed string is printed for each string one by one and in the same order as they are supplied to the standard output, any errors (there are a few) are printed to the standard error stream.

### Examples

```
$ torhash "Test"
16:E9CE2F53B6AAC5D460B77AC5EFCCA373847F80B6047CA91B61B8CED8BC
```

```
$ torhash "Foo" "Bar"
16:E9CE2F53B6AAC5D460A37D344256D0F922E1E79F90EB6607F999CC1720
16:C853176BCCA35C7B60C01AB75284FD11781C48B7F3CB28EB011007D6BA
```

## Technical Details

**The results produced by `torhash` are insecure because the source of random data (used as salt) is generated using the [`rand`](https://en.cppreference.com/w/c/numeric/random/rand) function (with a fixed seed) from the C standard library, as such it is not cryptographically secure.** This also causes the hashes to be the same as long as they are in the same position/order in the same system.

The hash itself is salted with random data and is computed according to the S2K (Iterated and Salted) algorithm in [RFC 2440](https://tools.ietf.org/html/rfc2440). [SHA-1](https://en.wikipedia.org/wiki/SHA-1) is used internally for the computation of the hash.

The output and other details of the final hashed string are defined in section 5.1 of the [control specification](https://gitweb.torproject.org/torspec.git/tree/control-spec.txt).

## Motivation

I have started work on this program because I hit a roadblock while working on another program ([ProxAllium](https://proxallium.dtw.tools), which is currently being [re-written](https://github.com/DcodingTheWeb/ProxAllium/tree/next-gen) in C). I had to make a choice between calling `tor --hash-password` to generate the hash for a password or generating the password in the program itself without relying on calling `tor`, for the latter I did not have enough technical understanding, so I began researching the documentation and source code for Tor. This program is the resulting fruit of that research.

This is intended to be a personal experiment to further develop my programming skills in C, this is also the first time I have used the CMake tool in the build process.

Thank you for reading down this far! If you are interested in my project involving Tor, I recommend checking out ProxAllium, which is a tool aimed for the average user to make using Tor as a proxy a bit easier, it also acts as a controller and several useful features are planned! Feel free to drop into the `#ProxAllium` IRC channel at OFTC to have a [chat](https://proxallium.dtw.tools/chat) :)
