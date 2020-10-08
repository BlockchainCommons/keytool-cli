# 🔑 Keytool

**Version 0.1.0**<br/>**September 28, 2020**

*Copyright © 2020 by Blockchain Commons, LLC*<br/>*Licensed under the "BSD-2-Clause Plus Patent License"*

---

Keytool is a CLI tool that implements a data flow graph for deriving cryptocurrency keys and addresses.

As its inputs, Keytool can be supplied with any of the attributes for the nodes, and as its outputs Keytool can be asked for any of the attribues of the existing or derivable nodes. In this way, Keytool can implement a very short derivation such as "Derive a master HD key from a seed," or a long sequence of derivations from a seed all the way to a payment address, also outputting any of the intermediate steps along the way.

## Derivations

Below are the single-step derivations that Keytool performs, in the form `A <- [B, C, ...]` where `A` is the derived attributed and `[B, C, ...]` are the attribues required to derive it.

If an attribute is listed more than once, then it has more than one derivation path. If the first derivation path cannot be used, the second one will be tried.

All the attributes except `entropy` are either derivable from other attributes or have a default value. The default values are suitable for use with Bitcoin mainnet. If these attributes are supplied on the command line, they override the defaults.

```
seed
asset (default: btc)
network <- [asset]
master-key <- [network, entropy]
purpose (default: 44 per BIP-44)
coin-type <- [asset]
account-index (default: 0)
account-derivation-path <- [purpose, coin-type, account-index]
account-key <- [master-key, account-derivation-path]
account-pub-key <- [account-key]
chain-type (default: external)
chain-type-int <- [chain-type]
address-index (default: 0)
address-derivation-path <- [account-derivation-path, chain-type-int, address-index]
address-key <- [master-key, address-derivation-path]
address-key <- [account-key, chain-type-int, address-index]
address-pub-key <- [address-key]
address-pub-key <- [account-pub-key, chain-type-int, address-index]
address-ec-key <- [address-key]
address-ec-key <- [address-ec-key-wif]
address-ec-key-wif <- [address-ec-key, network]
address-pub-ec-key <- [address-ec-key]
address-pub-ec-key <- [address-pub-key]
address-pkh <- [address-pub-ec-key, asset]
address-sh <- [address-pub-ec-key, asset]
```

These derivations are expressed visually in the graph below. Each node in the graph is a specific attribute that can be supplied directly on the command line, or derived from its predecessors. Two or more arrows entering a node indicate more than one possible set of inputs can be used to derive it. AND junctors indicate that *all* predecessor attributes must be supplied.

![](docs/Derivations.png)

## Formats

Keytool uses particular text formats for input and output attributes:

* HEX: A string of hexadecimal digits. Case-insensitive. The number of digits must be even.
* ENUM: A specific, defined enumerated value. For example `asset` can currently have the value `btc` (Bitcoin) or `btct` (Bitcoin testnet).
* INT: An integer.
* BIP32_PATH: A series of BIP-32 path elements, starting with `m` (for "master key") and followed by integers, which may optionally be followed by `h` (for hardened derivation). Example: `m/44h/0h/0h/0/123`.
* XPRV: A BIP-32 HD private key starting with `xprv`.
* XPUB: A BIP-32 HD public key starting with `xpub`.
* ECPRV: An elliptic curve private key in hex format.
* ECPUB: A compressed elliptic curve public key in hex format.
* WIF: An elliptic curve private key in Wallet Import Format.
* ADDRESS: An address in base-58 check format.

## Usage

```
$ keytool
    [--seed HEX]
    [--asset ENUM btc|btct]
    [--network ENUM mainnet|testnet]
    [--master-key XPRV]
    [--purpose INT]
    [--coin-type INT]
    [--account-index INT]
    [--account-derivation-path BIP32_PATH]
    [--account-key XPRV]
    [--account-pub-key XPUB]
    [--chain-type ENUM internal|external]
    [--chain-type-int INT]
    [--address-index INT]
    [--address-derivation-path BIP32_PATH]
    [--address-key XPRV]
    [--address-pub-key XPUB]
    [--address-ec-key ECPRV]
    [--address-ec-key-wif WIF]
    [--address-pub-ec-key ECPUB]
    [--address-pkh ADDRESS]
    [--address-sh ADDRESS]

    output-attributes...
```

## Example Usages

**Derive master HD key from seed:**

```
$ keytool \
    --seed 1eb5338edac6fae54cbb172091ae6c1a \
    --asset btc \
    master-key

xprv9s21ZrQH143K4FAunaSG9eYwrAaaChpSYwYF22eJiZJrz5zSBKTg7NJcFqWR2UZc7EhneSMJhHLmPsKx96UDgv9CdoLc6JfQo3AncKhYNSc
```

**Derive address #1234 of account #0 private key from master HD key:**

```
$ keytool \
    --master-key xprv9s21ZrQH143K4FAunaSG9eYwrAaaChpSYwYF22eJiZJrz5zSBKTg7NJcFqWR2UZc7EhneSMJhHLmPsKx96UDgv9CdoLc6JfQo3AncKhYNSc \
    --address-index 1234 \
    address-key

xprvA38UExqsbycjD3bUZYKHpPd8KtTKuJWciteEW5viseA2sXv55MYdib1YBdmN71Aq8jPHmDz9eFcM1pGRaGrweqUqjzBSLkRoxnre7N8u8jb
```

**Derive an arbitrary BIP32 private and public keys from master HD key:**

```
$ keytool \
    --master-key xprv9s21ZrQH143K4FAunaSG9eYwrAaaChpSYwYF22eJiZJrz5zSBKTg7NJcFqWR2UZc7EhneSMJhHLmPsKx96UDgv9CdoLc6JfQo3AncKhYNSc \
    --address-derivation-path m/99h/1h/2h/3 \
    address-key \
    address-pub-key

xprv9zvzy7RBcHUqSY5uPpz5jMgA4f98yCxW21Qrwg5qQu3GcqTSBsAgpRXfTnmE4SqSbLJVHx4NdWFEc1CzwVps15ynZtFPEtpaNh9uDkd3auw
xpub6DvMNcx5Sf38f2ANVrX66VctcgydNfgMPELTk4VSyEaFVdnajQUwNDr9K54VQ2SRM7gmcQmJcMa23xyWEwU2ehdU9tgFBozSUnLaWJAhmBk
```

* Note: Since `'` is a UNIX command-line quote mark, BIP32 paths given to Keytool use `h` to denote hardened derivations.

**Derive 10 successive bitcoin addreses from a seed:**

```
for (( i=0; i <= 9; ++i ))
do
    keytool \
    	--seed 1eb5338edac6fae54cbb172091ae6c1a \
    	--address-index $i \
    	address-index \
    	address-derivation-path \
    	address-pkh
done

0
m/44h/0h/0h/0/0
1M56Xw3jpmx2bdfvWH7HvJiXHyq9ybY6zq
1
m/44h/0h/0h/0/1
1J3DmbvZLACBkXxFpehuBDMziYmpRmM1cs
2
m/44h/0h/0h/0/2
1CbgRLCWsgRs92gfcZRcdu49AczLSnhE8v
3
m/44h/0h/0h/0/3
16qW5iyYtvB9ZB3hUjQBW9q99DYdqAALvc
4
m/44h/0h/0h/0/4
1FK9hTgVqAXh6Vgnf8p4t7UnzkdfKjH1T7
5
m/44h/0h/0h/0/5
1DG8MXbTgRPpjpKBw3ySwBh6mq2K9rPD8d
6
m/44h/0h/0h/0/6
16GB39n2RK3KF95nxdVCMM3PSeAXiB7hUu
7
m/44h/0h/0h/0/7
1JSyFfXprnofwZhB7X2DxN3WfQtk1Kibxs
8
m/44h/0h/0h/0/8
14mkCprpUs86Q8eUPQVwadhfNGpMcSPP67
9
m/44h/0h/0h/0/9
13LFSpGpeYjnFdbT4Sw8qHhCMKACAyoapR
```

Note the following: 

* When more than one output is requested, each output is printed in the order requested on a separate line.
* Supplied results, like `address-index`, or intermediate derived results, like `address-derivation-path` can also be requested.


## Providing Inputs via STDIN

When providing inputs to Keytool, any input values provided as an empty string are then expected via STDIN:

```
CLI:    $ keytool --seed '' master-key
STDIN:  8935a8068526d84da555cdb741a3b8a8
STDOUT: xprv9s21ZrQH143K3rJgfaUmUCueBxbDT1bbksxayh7ik1nZN41zAy2kQNop8REXEAAwqwes1E8rxk1313tyzsNmMq69h4vDyyrXkmjDZ2Nf3pN
```

You can of course use argument replacement with Seedtool to generate a seed, and from that a master key in one line:

```
$ keytool --seed `seedtool` master-key
xprv9s21ZrQH143K3SEi9YN67JXQxHRY4F2ignHZJxtVfMEh5D4NKcUYHrn75rZyt8nLMfzBc1enHGz9hqsbP98rp8i12DaiyYygUiss8YJ5qDC
```

Because you can provide inputs on STDIN, you can also use the shell to pipe the result of Seedtool to Keytool:

```
$ seedtool | keytool --seed '' master-key
xprv9s21ZrQH143K2QkBam5u1X5gwoLTvfk7QE2pfbr8ePDPNW4dZ6cmDtXgcRwjFBvPU1L8hC5o4nzxJB7L9hAzTY1VvHmo6M1EdTPfwxF15Bs
```

You can use Keytool to print out both the seed generated by Seedtool and the master key:

```
$ seedtool | keytool --seed '' seed master-key
9c802cc9fe2fee3fc9c75550f36dbc48
xprv9s21ZrQH143K2xbqevShzSjm4RCpeBgVfhM9enjHfrPpKnuH3f4NZYSHLFeoDYtRSQWCw1Df4xGkK2FjJQZTFYLNovKEJZrToCFMjH6xkp9
```

Other scripts can provide all the necessary inputs to keytool:

```
$ { seedtool && echo 'btct'; } | keytool --seed '' --asset '' master-key
tprv8ZgxMBicQKsPd1ieDbsts8QA9LzMbaeYE5pw7wL9k1QNteoZSbo62XK3HuP2WqByrRnBasP4jjEJUBf9mQSZVCCtMFAtSargNrByCkfFXh8
```

## Version History

### 0.1.0, 9/28/2020

* First testing release.
