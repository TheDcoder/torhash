# torhash

This is a simple implementation of the salted hash used by Tor's control port authentication for the "hashed password" method.

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
16:61A4F54AB65B0E336067885E98D86445AF710FC608B4189C4050938D71
```

```
$ torhash "Foo" "Bar"
16:CE4C870DAE845C0E60567EA825BB25AA8BC6066FE0AD67598874AAE102
16:673C130B93618DF060ECB37C89D9D22749125248122BE5113547D59769
```

## Technical Details

The hash itself is salted with random data and is computed according to the S2K (Iterated and Salted) algorithm in [RFC 2440](https://tools.ietf.org/html/rfc2440). [SHA-1](https://en.wikipedia.org/wiki/SHA-1) is used internally for the computation of the hash.

The output and other details of the final hashed string are defined in section 5.1 of the [control specification](https://gitweb.torproject.org/torspec.git/tree/control-spec.txt).

## Motivation

I have started work on this program because I hit a roadblock while working on another program ([ProxAllium](https://proxallium.dtw.tools), which is currently being [re-written](https://github.com/DcodingTheWeb/ProxAllium/tree/next-gen) in C). I had to make a choice between calling `tor --hash-password` to generate the hash for a password or generating the password in the program itself without relying on calling `tor`, for the latter I did not have enough technical understanding, so I began researching the documentation and source code for Tor. This program is the resulting fruit of that research.

This is intended to be a personal experiment to further develop my programming skills in C, this is also the first time I have used the CMake tool in the build process.

Thank you for reading down this far! If you are interested in my project involving Tor, I recommend checking out ProxAllium, which is a tool aimed for the average user to make using Tor as a proxy a bit easier, it also acts as a controller and several useful features are planned! Feel free to drop into the `#ProxAllium` IRC channel at OFTC to have a [chat](https://proxallium.dtw.tools/chat) :)
