# 🔑 Blockchain Commons keytool-cli

![Build](https://github.com/BlockchainCommons/keytool-cli/workflows/ci/badge.svg)
![Lint](https://github.com/BlockchainCommons/keytool-cli/workflows/lint/badge.svg)

### _by [Wolf McNally](https://www.github.com/wolfmcnally) and [Christopher Allen](https://www.github.com/ChristopherA)_

* <img src="https://github.com/BlockchainCommons/crypto-commons/blob/master/images/logos/crypto-commons-super-simple.png" width=16 valign="bottom">&nbsp;&nbsp; ***part of the [crypto commons](https://github.com/BlockchainCommons/crypto-commons/blob/master/README.md) technology family***
* <img src="https://github.com/BlockchainCommons/Gordian/blob/master/Images/logos/gordian-icon.png" width=16 valign="bottom"> ***part of the [gordian](https://github.com/BlockchainCommons/gordian/blob/master/README.md) technology family***

![](images/logos/keytool-screen.png)

## Introduction

`keytool` is a command-line tool that implements a data flow graph for deriving cryptocurrency keys and addresses, and signing transactions. Any of the nodes in the graph can be assigned and any set of nodes can be derived as long as their dependencies are met.

![](docs/Derivations.jpg)

Here are a few examples of use:

### Derive a BIP-32 master HD key from a cryptographic seed, and display it in UR and Base58 formats

```
$ keytool --seed 8935a8068526d84da555cdb741a3b8a8 master-key master-key-base58
ur:crypto-hdkey/oxadykaoykaxhdclaefgyaosptdlsrsggwtysbwfsbhppsuefxhftokkbadwtbrfcxjtoehgglyanlbwcsaahdcxqzaxhkwmmshphyiycpmkuyhdnekpweingrchkphtrktioedysagwzolesarniyzcrhdntkbb
xprv9s21ZrQH143K3rJgfaUmUCueBxbDT1bbksxayh7ik1nZN41zAy2kQNop8REXEAAwqwes1E8rxk1313tyzsNmMq69h4vDyyrXkmjDZ2Nf3pN
```

### Derive arbitrary BIP32 private and public keys from master HD key

```
$ keytool \
    --master-key xprv9s21ZrQH143K4FAunaSG9eYwrAaaChpSYwYF22eJiZJrz5zSBKTg7NJcFqWR2UZc7EhneSMJhHLmPsKx96UDgv9CdoLc6JfQo3AncKhYNSc \
    --full-address-derivation-path m/99h/1h/2h/3 \
    address-key-base58 \
    address-pub-key-base58

xprv9zvzy7RBcHUqSY5uPpz5jMgA4f98yCxW21Qrwg5qQu3GcqTSBsAgpRXfTnmE4SqSbLJVHx4NdWFEc1CzwVps15ynZtFPEtpaNh9uDkd3auw
xpub6DvMNcx5Sf38f2ANVrX66VctcgydNfgMPELTk4VSyEaFVdnajQUwNDr9K54VQ2SRM7gmcQmJcMa23xyWEwU2ehdU9tgFBozSUnLaWJAhmBk
```

### Derive 10 successive bitcoin addreses from a seed.

```
for (( i=0; i <= 9; ++i ))
do
    keytool \
    	--seed 1eb5338edac6fae54cbb172091ae6c1a \
    	--address-index $i \
    	address-index \
    	full-address-derivation-path \
    	address-pkh
done

0
c1859926/84h/0h/0h/0/0
17u4HStkGWxJ5PtNqzCMEKRzaPJ8h35eBc
1
c1859926/84h/0h/0h/0/1
1JyxrbHYS7ZGhq68JewhJAMWka38Bv8tFw
2
c1859926/84h/0h/0h/0/2
1Agp5QYUPXWT2PLQtqHezQB8oChEBCTmnk
3
c1859926/84h/0h/0h/0/3
1Jauyj8uiz5YwWKq6kcP5jNKxEigLYAo35
4
c1859926/84h/0h/0h/0/4
15ccuKzfmJQ6GSLzbgcUYwjKkkEdKt1ELf
5
c1859926/84h/0h/0h/0/5
1CTnrP3Pwrd3Xf77rvh4XuhJJDpyiV2PGe
6
c1859926/84h/0h/0h/0/6
19pbYMfrNUJesmyUTxVxHVHRfFwkRhPXmK
7
c1859926/84h/0h/0h/0/7
141Wb2o9u22XFiiQXo4RQhCWRefKiUU8GJ
8
c1859926/84h/0h/0h/0/8
171m44xH6vs5yJmRdY8mEf8zVAUr5ka8SA
9
c1859926/84h/0h/0h/0/9
1BHXRjigSLZLt8Z7Rss27u6kZA17Pptpga
```

### Add a signature to a partially-signed Bitcoin transaction (PSBT).

```
keytool \
  --network testnet \
  --psbt "cHNidP8BAMMCAAAAA0DcshmpIS/ZW94f7roto1MhBNhk6Rz/ZaetRRfcedh9AAAAAAD9////wm+mbFIbTYiPlNNhB8P1/vBcwKG/ZryOHMKkL7dSqTYAAAAAAP3///+CmZgUyFrdKNCZUoqfgBhCAFORPdiP/qr0OOt4I5n+CgAAAAAA/f///wJAQg8AAAAAABYAFCBk78yn2RApIqWfuXigrti3WcQRncYQAAAAAAAWABSMyh/Y85oXJKUZQJT+IeAZJk/MeAAAAAAAAQD9PgECAAAABnPYBYJW735g7YMuaFrrK2ZgQj2XELyHukIJmdvOUB4sAAAAAAD+////bmU42jLXSFq3iLHmsyOR/OrF/x0ZQ2lAbecy8CeanlsAAAAAAP7///87WWkGf++EI4Um41R9YUGj00Z0+wvW0fmPYJ+81lWZOAEAAAAA/v///++syYWL4jBkYbhPlOrxJkozfdkL/XJWnhl2nVzVxa2OAAAAAAD+////UAL64CRRErlwBwQM+X5opqpiI0JmpsKs2a1MzGtZh8QAAAAAAP7///8chEjupyVpNkPdCyBE1wpPrS8MGI9EjEGpj7CTgZAXtgAAAAAA/v///wKghgEAAAAAABYAFMzoxnRhxnGxMSMykm4rhwto1dz6vkMPAAAAAAAWABTzi7rfXv1coXc7HvFo2QoeYq4U1E54HAABAR+ghgEAAAAAABYAFMzoxnRhxnGxMSMykm4rhwto1dz6IgYCtKMsZPEI+kb9daMTFeF/+8io6poDvTx+cinpPo9y1EQYDwVpQ1QAAIABAACAAAAAgAAAAAAQAAAAAAEAUgIAAAABbEc0zoXZ/16+PtvwSQ43BytcwpYvO6gGU11xDcaGH7YAAAAAAAEAAAABgEEPAAAAAAAWABQMEbJbdDyzQi5GH1/cPH4QeuPCiwAAAAABAR+AQQ8AAAAAABYAFAwRslt0PLNCLkYfX9w8fhB648KLIgYDcV7jKmCV/4gwmyIZjcVj1LXh1/M4er/DpkPbYiUikt8YDwVpQ1QAAIABAACAAAAAgAAAAACIAAAAAAEAUgIAAAABtvIk4LNoWJem4tfDI5qeVZKR954vLCVJBiqv3pSFR8AAAAAAAAEAAAAB0kEPAAAAAAAWABT93jySCCHYPSeMHxs0jj9i+0UtmAAAAAABAR/SQQ8AAAAAABYAFP3ePJIIIdg9J4wfGzSOP2L7RS2YIgYDhA94BGdguQJPUq61nFoMNrVM4VUaSEJwM7lZcPGyFPUYDwVpQ1QAAIABAACAAAAAgAAAAACGAAAAACICAgFa3FuEwDkszXFDVxN6KU6pVUV/f6d1JSWZ1RsD4CNtGA8FaUNUAACAAQAAgAAAAIAAAAAAiQAAAAAiAgJZ8CLw/Bdus5hyuOPx7cnFfkBkRoTavnS6BRAHi5GOLBgPBWlDVAAAgAEAAIAAAACAAQAAAFIAAAAA" \
  --address-ec-key-wif "cSyLgU8rSvYNU1j6XR2vo5ebSQqza7PR9iFNtkkYrFMwyB5or5gH" \
  psbt-signed-base64

cHNidP8BAMMCAAAAA0DcshmpIS/ZW94f7roto1MhBNhk6Rz/ZaetRRfcedh9AAAAAAD9////wm+mbFIbTYiPlNNhB8P1/vBcwKG/ZryOHMKkL7dSqTYAAAAAAP3///+CmZgUyFrdKNCZUoqfgBhCAFORPdiP/qr0OOt4I5n+CgAAAAAA/f///wJAQg8AAAAAABYAFCBk78yn2RApIqWfuXigrti3WcQRncYQAAAAAAAWABSMyh/Y85oXJKUZQJT+IeAZJk/MeAAAAAAAAQD9PgECAAAABnPYBYJW735g7YMuaFrrK2ZgQj2XELyHukIJmdvOUB4sAAAAAAD+////bmU42jLXSFq3iLHmsyOR/OrF/x0ZQ2lAbecy8CeanlsAAAAAAP7///87WWkGf++EI4Um41R9YUGj00Z0+wvW0fmPYJ+81lWZOAEAAAAA/v///++syYWL4jBkYbhPlOrxJkozfdkL/XJWnhl2nVzVxa2OAAAAAAD+////UAL64CRRErlwBwQM+X5opqpiI0JmpsKs2a1MzGtZh8QAAAAAAP7///8chEjupyVpNkPdCyBE1wpPrS8MGI9EjEGpj7CTgZAXtgAAAAAA/v///wKghgEAAAAAABYAFMzoxnRhxnGxMSMykm4rhwto1dz6vkMPAAAAAAAWABTzi7rfXv1coXc7HvFo2QoeYq4U1E54HAABAR+ghgEAAAAAABYAFMzoxnRhxnGxMSMykm4rhwto1dz6IgICtKMsZPEI+kb9daMTFeF/+8io6poDvTx+cinpPo9y1ERIMEUCIQDkmw5j/7puaE9pcUNrOKe4HLpXgP2weErNEsFFJUWQYAIgKAqlZqukvHiRGjZysMu1GAGJRgzZxyjdEan3EZTRrGwBIgYCtKMsZPEI+kb9daMTFeF/+8io6poDvTx+cinpPo9y1EQYDwVpQ1QAAIABAACAAAAAgAAAAAAQAAAAAAEAUgIAAAABbEc0zoXZ/16+PtvwSQ43BytcwpYvO6gGU11xDcaGH7YAAAAAAAEAAAABgEEPAAAAAAAWABQMEbJbdDyzQi5GH1/cPH4QeuPCiwAAAAABAR+AQQ8AAAAAABYAFAwRslt0PLNCLkYfX9w8fhB648KLIgYDcV7jKmCV/4gwmyIZjcVj1LXh1/M4er/DpkPbYiUikt8YDwVpQ1QAAIABAACAAAAAgAAAAACIAAAAAAEAUgIAAAABtvIk4LNoWJem4tfDI5qeVZKR954vLCVJBiqv3pSFR8AAAAAAAAEAAAAB0kEPAAAAAAAWABT93jySCCHYPSeMHxs0jj9i+0UtmAAAAAABAR/SQQ8AAAAAABYAFP3ePJIIIdg9J4wfGzSOP2L7RS2YIgYDhA94BGdguQJPUq61nFoMNrVM4VUaSEJwM7lZcPGyFPUYDwVpQ1QAAIABAACAAAAAgAAAAACGAAAAACICAgFa3FuEwDkszXFDVxN6KU6pVUV/f6d1JSWZ1RsD4CNtGA8FaUNUAACAAQAAgAAAAIAAAAAAiQAAAAAiAgJZ8CLw/Bdus5hyuOPx7cnFfkBkRoTavnS6BRAHi5GOLBgPBWlDVAAAgAEAAIAAAACAAQAAAFIAAAAA
```

### Convert a PSBT from Base-64 to [Uniform Resource (UR)](https://github.com/BlockchainCommons/Research/blob/master/papers/bcr-2020-005-ur.md) format:

```
keytool \
	--psbt "cHNidP8BAHUCAAAAASaBcTce3/KF6Tet7qSze3gADAVmy7OtZGQXE8pCFxv2AAAAAAD+////AtPf9QUAAAAAGXapFNDFmQPFusKGh2DpD9UhpGZap2UgiKwA4fUFAAAAABepFDVF5uM7gyxHBQ8k0+65PJwDlIvHh7MuEwAAAQD9pQEBAAAAAAECiaPHHqtNIOA3G7ukzGmPopXJRjr6Ljl/hTPMti+VZ+UBAAAAFxYAFL4Y0VKpsBIDna89p95PUzSe7LmF/////4b4qkOnHf8USIk6UwpyN+9rRgi7st0tAXHmOuxqSJC0AQAAABcWABT+Pp7xp0XpdNkCxDVZQ6vLNL1TU/////8CAMLrCwAAAAAZdqkUhc/xCX/Z4Ai7NK9wnGIZeziXikiIrHL++E4sAAAAF6kUM5cluiHv1irHU6m80GfWx6ajnQWHAkcwRAIgJxK+IuAnDzlPVoMR3HyppolwuAJf3TskAinwf4pfOiQCIAGLONfc0xTnNMkna9b7QPZzMlvEuqFEyADS8vAtsnZcASED0uFWdJQbrUqZY3LLh+GFbTZSYG2YVi/jnF6efkE/IQUCSDBFAiEA0SuFLYXc2WHS9fSrZgZU327tzHlMDDPOXMMJ/7X85Y0CIGczio4OFyXBl/saiK9Z9R5E5CVbIBZ8hoQDHAXR8lkqASECI7cr7vCWXRC+B3jv7NYfysb3mk6haTkzgHNEZPhPKrMAAAAAAAAA" \
	psbt

ur:crypto-psbt/hkaodnjojkidjyzmadaekpaoaeaeaeaddslyjsemckurwzlpwlempmwyoxqdkgksaebnahiysbqdpmieiechbwsgfwchcwynaeaeaeaeaezezmzmzmaoteurykahaeaeaeaecfkoptbbtisknlaxskrdsalnlthnwlbstlcloxiyhtosihcxlopsaevyykahaeaeaeaechptbbecfevavlfrlsdwflahbsdktewyrhfnnsaxmwlustltqddmbwaeaeadaezconadadaeaeaeaeadaoldotstckpygtcxvtemcwrkoxsfinmyoemdsofgftzsdmeslblpeosfrpdlmdiovwadaeaeaechcmaebbrncsttgmptpfbgaxntpefsosuegwgueennwprhlpzmzmzmzmlnyapkfxoscazmbbfdldftgubkjpemwsjefgayrkprutdpadjsvaftwpimfdmhqzadaeaeaechcmaebbzefmnnwnosfewljytaaossechkfxpysbeeryguguzmzmzmzmaoaesawmbdaeaeaeaecfkoptbblptkwnaslbtavtayrkeepejonsidcfkgetmslefdlopsjpzeyagldwaeaeaechptbbeomsdardclwstbdrstguptrftiiotbstolotntahltaofldyfyaocxdibgrncpvtdibsesgwhflsbyuokeptolldjoroaoheutfrdkaodtwtlbleheftdkaocxadluettsuotebbvdeesodijetbzofzynjkeyhpssrdoyfyspaetdwzwtdpprkohhadclaxtdvyhfjymwcwpmgenliajpsbltvylpjnengmhnjnmkhfdlvlnshynnkbfpfhclahaofddyfeaoclaettdnlpdplpuotahstdykwkpyiyamghurjtwesfkkgsbneotohhsraszmreztvwlgaocxioeolemnbachdasemszocylopehkykckfyvedahpcxcmkelnlraxceahttwzhkdradclaocnrldnwywtmthlbernatkswswptbctsgswylnygloyineseolajkfyieyagwdrqdaeaeaeaeaeaeaeswsezose
```

### Convert the same PSBT from UR to Base64:

```
keytool \
	--psbt ur:crypto-psbt/hkaodnjojkidjyzmadaekpaoaeaeaeaddslyjsemckurwzlpwlempmwyoxqdkgksaebnahiysbqdpmieiechbwsgfwchcwynaeaeaeaeaezezmzmzmaoteurykahaeaeaeaecfkoptbbtisknlaxskrdsalnlthnwlbstlcloxiyhtosihcxlopsaevyykahaeaeaeaechptbbecfevavlfrlsdwflahbsdktewyrhfnnsaxmwlustltqddmbwaeaeadaezconadadaeaeaeaeadaoldotstckpygtcxvtemcwrkoxsfinmyoemdsofgftzsdmeslblpeosfrpdlmdiovwadaeaeaechcmaebbrncsttgmptpfbgaxntpefsosuegwgueennwprhlpzmzmzmzmlnyapkfxoscazmbbfdldftgubkjpemwsjefgayrkprutdpadjsvaftwpimfdmhqzadaeaeaechcmaebbzefmnnwnosfewljytaaossechkfxpysbeeryguguzmzmzmzmaoaesawmbdaeaeaeaecfkoptbblptkwnaslbtavtayrkeepejonsidcfkgetmslefdlopsjpzeyagldwaeaeaechptbbeomsdardclwstbdrstguptrftiiotbstolotntahltaofldyfyaocxdibgrncpvtdibsesgwhflsbyuokeptolldjoroaoheutfrdkaodtwtlbleheftdkaocxadluettsuotebbvdeesodijetbzofzynjkeyhpssrdoyfyspaetdwzwtdpprkohhadclaxtdvyhfjymwcwpmgenliajpsbltvylpjnengmhnjnmkhfdlvlnshynnkbfpfhclahaofddyfeaoclaettdnlpdplpuotahstdykwkpyiyamghurjtwesfkkgsbneotohhsraszmreztvwlgaocxioeolemnbachdasemszocylopehkykckfyvedahpcxcmkelnlraxceahttwzhkdradclaocnrldnwywtmthlbernatkswswptbctsgswylnygloyineseolajkfyieyagwdrqdaeaeaeaeaeaeaeswsezose \
	psbt-base64

cHNidP8BAHUCAAAAASaBcTce3/KF6Tet7qSze3gADAVmy7OtZGQXE8pCFxv2AAAAAAD+////AtPf9QUAAAAAGXapFNDFmQPFusKGh2DpD9UhpGZap2UgiKwA4fUFAAAAABepFDVF5uM7gyxHBQ8k0+65PJwDlIvHh7MuEwAAAQD9pQEBAAAAAAECiaPHHqtNIOA3G7ukzGmPopXJRjr6Ljl/hTPMti+VZ+UBAAAAFxYAFL4Y0VKpsBIDna89p95PUzSe7LmF/////4b4qkOnHf8USIk6UwpyN+9rRgi7st0tAXHmOuxqSJC0AQAAABcWABT+Pp7xp0XpdNkCxDVZQ6vLNL1TU/////8CAMLrCwAAAAAZdqkUhc/xCX/Z4Ai7NK9wnGIZeziXikiIrHL++E4sAAAAF6kUM5cluiHv1irHU6m80GfWx6ajnQWHAkcwRAIgJxK+IuAnDzlPVoMR3HyppolwuAJf3TskAinwf4pfOiQCIAGLONfc0xTnNMkna9b7QPZzMlvEuqFEyADS8vAtsnZcASED0uFWdJQbrUqZY3LLh+GFbTZSYG2YVi/jnF6efkE/IQUCSDBFAiEA0SuFLYXc2WHS9fSrZgZU327tzHlMDDPOXMMJ/7X85Y0CIGczio4OFyXBl/saiK9Z9R5E5CVbIBZ8hoQDHAXR8lkqASECI7cr7vCWXRC+B3jv7NYfysb3mk6haTkzgHNEZPhPKrMAAAAAAAAA
```

## Full Documentation

See [`MANUAL.md`](MANUAL.md) for details, more examples, and version history.

## Dependencies

* [`libwally-core`](https://github.com/BlockchainCommons/bc-libwally-core)
* [`GNU argp`](https://www.gnu.org/software/libc/manual/html_node/Argp.html)

## Recommended Installation

These dependencies are automatically installed as submodules when you run the build script. This is the recommended way to install.

### Build with Docker

[Install docker](https://docs.docker.com/get-docker/) and run:

```bash
# Build the image
$ docker build -t keytool-cli .
# Run the container
$ docker run --rm -it keytool-cli --help
```

### MacOS

```bash
$ brew install autoconf automake gnu-sed libtool
```

You must then download or clone this repo. Afterward, cd into the repo directory and:

```bash
$ ./build.sh
$ sudo make install
```

#### ⚠️ Conflict with Java `keytool`

The macOS Java JDK includes a command-line tool also called `keytool`. If after installing Blockchain Commons `keytool` you type the commands below and see the corresponding output, then you are successfully running what you just installed:

```sh
$ which keytool
/usr/local/bin/keytool

$ keytool
keytool: No outputs requested.
Try 'keytool --help' or 'keytool --usage' for more information.
```

However, if you see:

```sh
$ which keytool
/usr/bin/keytool

$ keytool
Key and Certificate Management Tool
...
```

Then the shell is finding Java's `keytool` before ours. You will need to edit your `.zprofile` (zsh) or `.bashprofile` (bash) file to include the install path of Blockchain Commons `keytool` before `/usr/bin`:

```sh
PATH=/usr/local/bin:$PATH
```

### Build on Linux

Make sure you have `llvm`/`clang`, `libc++` and `libc++abi` installed, all with
a minimum recommended version 10.

#### Build on Ubuntu and Debian

First, install the dependencies:

```bash
$ sudo apt-get update

$ sudo apt install lsb-release wget software-properties-common git apt-transport-https
$ wget https://apt.llvm.org/llvm.sh
$ chmod +x llvm.sh
$ sudo ./llvm.sh 10  # version 10
$ sudo apt-get -y install build-essential pkg-config autoconf libtool libc++-10-dev libc++abi-10-dev python
```

Then, you can clone and build this repo:

```bash
$ git clone https://github.com/BlockchainCommons/keytool-cli.git
$ cd keytool-cli/
$ export CC="clang-10" && export CXX="clang++-10" && ./build.sh
$ sudo make install
```

### Build on Windows

See [instructions here](docs/Install-Windows.md).

## Alternative Installation

This sequence does *not* install the dependencies from submodules; instead they must be installed in the usual places on the build system, otherwise the `./configure` step below will fail.

```bash
$ ./configure
$ make
$ sudo make install
```
*Note:* On Linux the first step is `./configure CC=clang-10 CXX=clang++-10`

## Incremental Builds

If you wish to make changes to the source code and rebuild:

```bash
# Make source changes
$ source set_build_paths.sh # sets shell variables used by make
$ make clean # If you want a clean build
$ make
```

## Notes for Maintainers

Before accepting a PR that can affect build or unit tests, make sure the following sequence of commands succeeds:

```bash
$ ./build.sh
$ make lint
$ make check
$ make distclean
```

`make lint` uses [Cppcheck](https://en.wikipedia.org/wiki/Cppcheck) to perform static analysis on the code. All PRs should pass with no warnings.

## Origin, Authors, Copyright & Licenses

Unless otherwise noted (either in this [/README.md](./README.md) or in the file's header comments) the contents of this repository are Copyright © 2020 by Blockchain Commons, LLC, and are [licensed](./LICENSE) under the [spdx:BSD-2-Clause Plus Patent License](https://spdx.org/licenses/BSD-2-Clause-Patent.html).

In most cases, the authors, copyright, and license for each file reside in header comments in the source code. When it does not we have attempted to attribute it accurately in the table below.

This table below also establishes provenance (repository of origin, permalink, and commit id) for files included from repositories that are outside of this repository. Contributors to these files are listed in the commit history for each repository, first with changes found in the commit history of this repo, then in changes in the commit history of their repo of their origin.

| File      | From                                                         | Commit                                                       | Authors & Copyright (c)                                | License                                                     |
| --------- | ------------------------------------------------------------ | ------------------------------------------------------------ | ------------------------------------------------------ | ----------------------------------------------------------- |
| randombytes.c | [dsprenkels/randombytes](https://github.com/dsprenkels/randombytes/blob/master/randombytes.c) | [6db39aa](https://github.com/dsprenkels/randombytes/commit/6db39aaae6bb9ab97beca00d81bcfe935c56c88d) | 2017-2019 [Daan Sprenkels](https://github.com/dsprenkels/) | [MIT](https://github.com/dsprenkels/randombytes/commit/73ae9b4fce2e62babdd6a480b53ad449dd745ed9) |
| randombytes.h | [dsprenkels/randombytes](https://github.com/dsprenkels/randombytes/blob/master/randombytes.h) | [19fd002](https://github.com/dsprenkels/randombytes/commit/19fd002d9b7b001b333a671186a91231b60d821b) | 2017-2019 [Daan Sprenkels](https://github.com/dsprenkels/) | [MIT](https://github.com/dsprenkels/randombytes/commit/73ae9b4fce2e62babdd6a480b53ad449dd745ed9) |
| base64 codec | [ReneNyffenegger/cpp-base64](https://github.com/ReneNyffenegger/cpp-base64) | [80525f9](https://github.com/ReneNyffenegger/cpp-base64/commit/80525f93cbabaa11bbcae137f24467f97d74345d) | 2004-2017, 2020 [René Nyffenegger](https://github.com/ReneNyffenegger) | [ZLib](https://github.com/ReneNyffenegger/cpp-base64/blob/master/LICENSE) |

### Tool Dependencies

To build `keytool` you'll need to use the following tools:

- autotools - Gnu Build System from Free Software Foundation ([intro](https://www.gnu.org/software/automake/manual/html_node/Autotools-Introduction.html)).

## Financial Support

Seedtool is a project of [Blockchain Commons](https://www.blockchaincommons.com/). We are proudly a "not-for-profit" social benefit corporation committed to open source & open development. Our work is funded entirely by donations and collaborative partnerships with people like you. Every contribution will be spent on building open tools, technologies, and techniques that sustain and advance blockchain and internet security infrastructure and promote an open web.

To financially support further development of Seedtool and other projects, please consider becoming a Patron of Blockchain Commons through ongoing monthly patronage as a [GitHub Sponsor](https://github.com/sponsors/BlockchainCommons). You can also support Blockchain Commons with bitcoins at our [BTCPay Server](https://btcpay.blockchaincommons.com/).

## Contributing

We encourage public contributions through issues and pull-requests! Please review [CONTRIBUTING.md](./CONTRIBUTING.md) for details on our development process. All contributions to this repository require a GPG signed [Contributor License Agreement](./CLA.md).

### Discussions

The best place to talk about Blockchain Commons and its projects is in our GitHub Discussions areas.

[**Gordian User Community**](https://github.com/BlockchainCommons/Gordian/discussions). For users of the Gordian reference apps, including [Gordian Coordinator](https://github.com/BlockchainCommons/iOS-GordianCoordinator), [Gordian Seed Tool](https://github.com/BlockchainCommons/GordianSeedTool-iOS), [Gordian Server](https://github.com/BlockchainCommons/GordianServer-macOS), [Gordian Wallet](https://github.com/BlockchainCommons/GordianWallet-iOS), and [SpotBit](https://github.com/BlockchainCommons/spotbit) as well as our whole series of [CLI apps](https://github.com/BlockchainCommons/Gordian/blob/master/Docs/Overview-Apps.md#cli-apps). This is a place to talk about bug reports and feature requests as well as to explore how our reference apps embody the [Gordian Principles](https://github.com/BlockchainCommons/Gordian#gordian-principles).

[**Blockchain Commons Discussions**](https://github.com/BlockchainCommons/Community/discussions). For developers, interns, and patrons of Blockchain Commons, please use the discussions area of the [Community repo](https://github.com/BlockchainCommons/Community) to talk about general Blockchain Commons issues, the intern program, or topics other than those covered by the [Gordian Developer Community](https://github.com/BlockchainCommons/Gordian-Developer-Community/discussions) or the 
[Gordian User Community](https://github.com/BlockchainCommons/Gordian/discussions).


### Other Questions & Problems

As an open-source, open-development community, Blockchain Commons does not have the resources to provide direct support of our projects. Please consider the discussions area as a locale where you might get answers to questions. Alternatively, please use this repository's [issues](./issues) feature. Unfortunately, we can not make any promises on response time.

If your company requires support to use our projects, please feel free to contact us directly about options. We may be able to offer you a contract for support from one of our contributors, or we might be able to point you to another entity who can offer the contractual support that you need.

### Credits

The following people directly contributed to this repository. You can add your name here by getting involved — the first step is to learn how to contribute from our [CONTRIBUTING.md](./CONTRIBUTING.md) documentation.

| Name              | Role                | Github                                            | Email                                 | GPG Fingerprint                                    |
| ----------------- | ------------------- | ------------------------------------------------- | ------------------------------------- | -------------------------------------------------- |
| Christopher Allen | Principal Architect | [@ChristopherA](https://github.com/ChristopherA) | \<ChristopherA@LifeWithAlacrity.com\> | FDFE 14A5 4ECB 30FC 5D22  74EF F8D3 6C91 3574 05ED |
| Wolf McNally      | Project Lead        | [@WolfMcNally](https://github.com/wolfmcnally)    | \<Wolf@WolfMcNally.com\>              | 9436 52EE 3844 1760 C3DC  3536 4B6C 2FCF 8947 80AE |

## Responsible Disclosure

We want to keep all our software safe for everyone. If you have discovered a security vulnerability, we appreciate your help in disclosing it to us in a responsible manner. We are unfortunately not able to offer bug bounties at this time.

We do ask that you offer us good faith and use best efforts not to leak information or harm any user, their data, or our developer community. Please give us a reasonable amount of time to fix the issue before you publish it. Do not defraud our users or us in the process of discovery. We promise not to bring legal action against researchers who point out a problem provided they do their best to follow the these guidelines.

## Reporting a Vulnerability

Please report suspected security vulnerabilities in private via email to ChristopherA@BlockchainCommons.com (do not use this email for support). Please do NOT create publicly viewable issues for suspected security vulnerabilities.

The following keys may be used to communicate sensitive information to developers:

| Name              | Fingerprint                                        |
| ----------------- | -------------------------------------------------- |
| Christopher Allen | FDFE 14A5 4ECB 30FC 5D22  74EF F8D3 6C91 3574 05ED |

You can import a key by running the following command with that individual’s fingerprint: `gpg --recv-keys "<fingerprint>"` Ensure that you put quotes around fingerprints that contain spaces.
