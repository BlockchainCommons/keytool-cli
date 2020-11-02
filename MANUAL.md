# 🔑 Keytool

**Version 0.3.0**<br/>**November 2, 2020**

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
master-key <- [network, seed]
master-key-fingerprint <- [master-key]
output-type (default: wpkh)
purpose <- [output-type]
coin-type <- [asset]
account-index (default: 0)
account-derivation-path <- [master-key-fingerprint, purpose, coin-type, account-index]
account-key <- [master-key, account-derivation-path]
account-pub-key <- [account-key]
chain-type (default: external)
chain-type-int <- [chain-type];
address-index (default: 0)
address-derivation-path <- [chain-type-int, address-index]
full-address-derivation-path <- [account-derivation-path, address-derivation-path]
address-key <- [master-key, full-address-derivation-path]
address-key <- [account-key, address-derivation-path]
address-pub-key <- [address-key]
address-pub-key <- [account-pub-key, address-derivation-path]
address-ec-key <- [address-key]
address-ec-key <- [address-ec-key-wif, network]
address-ec-key-wif <- [address-ec-key, network]
address-pub-ec-key <- [address-ec-key]
address-pub-ec-key <- [address-pub-key]
address-pkh <- [address-pub-ec-key, asset]
address-sh <- [address-pub-ec-key, asset]
address-segwit <- [address-pub-key, network]
output-descriptor <- [output-type, account-derivation-path, address-derivation-path, account-pub-key]
psbt
psbt-finalized <- [psbt]
transaction <- [psbt-finalized]
psbt-signed <- [psbt, address-ec-key]
```

These derivations are expressed visually in the graph below. Each node in the graph is a specific attribute that can be supplied directly on the command line, or derived from its predecessors. Two or more arrows entering a node indicate more than one possible set of inputs can be used to derive it. AND junctors indicate that *all* predecessor attributes must be supplied, unless a predecessor is marked `optional`.

![](docs/Derivations.jpg)

## Formats

Keytool uses particular text formats for input and output attributes:

* HEX: A string of hexadecimal digits, case-insensitive.
* ENUM: A specific, defined enumerated value. For example `asset` can currently have the value `btc` (Bitcoin) or `btct` (Bitcoin testnet).
* INDEX: A non-negative integer.
* INDEX_BOUND: An INDEX or the wildcard `*` (note that on the Unix command line, you will need to enclose `*` in single quotes to avoid shell globbing.)-
* BIP32_PATH: A series of BIP-32 path elements, starting with `m` (for "master key") and followed by integers, which may optionally be followed by `h` (for hardened derivation). Example: `m/44h/0h/0h/0/123`.
* XPRV: A BIP-32 HD private key starting with `xprv`.
* XPUB: A BIP-32 HD public key starting with `xpub`.
* ECPRV: An elliptic curve private key in hex format.
* ECPUB: A compressed elliptic curve public key in hex format.
* WIF: An elliptic curve private key in Wallet Import Format.
* ADDRESS: An address in base-58 check format.
* BASE64: A partially-signed Bitcoin transaction (PSBT).
* TRANSACTION: A raw Bitcoin transaction.

## Usage

```
keytool
  [-?V] [--account-derivation-path=BIP32_PATH]
  [--account-index=INDEX] 
  [--account-key=XPRV]
  [--account-pub-key=XPUB] 
  [--address-derivation-path=BIP32_PATH]
  [--address-ec-key=ECPRV] 
  [--address-ec-key-wif=WIF]
  [--address-index=INDEX_BOUND] 
  [--address-key=XPRV]
  [--address-pkh=ADDRESS] 
  [--address-pub-ec-key=ECPUB]
  [--address-pub-key=XPUB] 
  [--address-segwit=ADDRESS]
  [--address-sh=ADDRESS] 
  [--asset=ENUM btc|btct] 
  [--chain-type=ENUM internal|external|identity] 
  [--chain-type-int=INDEX]
  [--coin-type=INDEX] 
  [--full-address-derivation-path=BIP32_PATH]
  [--master-key=XPRV] 
  [--master-key-fingerprint=HEX] 
  [--network=ENUM mainnet|testnet] 
  [--output-descriptor=OUTPUT_DESCRIPTOR]
  [--output-type=ENUM pkh|wpkh|sh-wpkh] 
  [--psbt=BASE64]
  [--psbt-finalized=BASE64] 
  [--psbt-signed=BASE64] 
  [--purpose=INDEX]
  [--seed=HEX] 
  [--transaction=HEX] 
  [--help] 
  [--usage]
  [--version]
              
  output-attributes...
```

## Example Usages

### Derive master HD key from seed

```
$ keytool \
    --seed 1eb5338edac6fae54cbb172091ae6c1a \
    --asset btc \
    master-key

xprv9s21ZrQH143K4FAunaSG9eYwrAaaChpSYwYF22eJiZJrz5zSBKTg7NJcFqWR2UZc7EhneSMJhHLmPsKx96UDgv9CdoLc6JfQo3AncKhYNSc
```

### Derive address #1234 of account #0 private key from master HD key

```
$ keytool \
    --master-key xprv9s21ZrQH143K4FAunaSG9eYwrAaaChpSYwYF22eJiZJrz5zSBKTg7NJcFqWR2UZc7EhneSMJhHLmPsKx96UDgv9CdoLc6JfQo3AncKhYNSc \
    --address-index 1234 \
    address-key

xprvA33qYVBGLwEiCQNFTTpga5EHqJy1EnswzgzuSsBC3tPqz29qQ8PGDwyNuwearjiReYB8DK7nbJj52gKLjvGXsUx7RqmqtVgaYMun9CCQ3h8
```

### Derive an arbitrary BIP32 private and public keys from master HD key

```
$ keytool \
    --master-key xprv9s21ZrQH143K4FAunaSG9eYwrAaaChpSYwYF22eJiZJrz5zSBKTg7NJcFqWR2UZc7EhneSMJhHLmPsKx96UDgv9CdoLc6JfQo3AncKhYNSc \
    --full-address-derivation-path m/99h/1h/2h/3 \
    address-key \
    address-pub-key

xprv9zvzy7RBcHUqSY5uPpz5jMgA4f98yCxW21Qrwg5qQu3GcqTSBsAgpRXfTnmE4SqSbLJVHx4NdWFEc1CzwVps15ynZtFPEtpaNh9uDkd3auw
xpub6DvMNcx5Sf38f2ANVrX66VctcgydNfgMPELTk4VSyEaFVdnajQUwNDr9K54VQ2SRM7gmcQmJcMa23xyWEwU2ehdU9tgFBozSUnLaWJAhmBk
```

* Note: Since `'` is a UNIX command-line quote mark, BIP32 paths given to Keytool use `h` to denote hardened derivations.

### Derive 10 successive bitcoin addreses from a seed

```
for (( i=0; i <= 9; ++i ))
do
    src/keytool \
        --asset btct \
    	--seed e48225bd3c8d508f45a86607a8eca277 \
    	--address-index $i \
    	address-index \
    	full-address-derivation-path \
    	address-segwit
done

0
3aa00dc3/84h/1h/0h/0/0
tb1qtc425u2januzcjk3x39vp24dwkawsqu8v5jn53
1
3aa00dc3/84h/1h/0h/0/1
tb1qynvun8gs4e4xtegfs4jdymzmt4wynph895dk68
2
3aa00dc3/84h/1h/0h/0/2
tb1qhs4j4ytplrynu8kqmk6v40del62dmklu02lxtv
3
3aa00dc3/84h/1h/0h/0/3
tb1qcdnerx0he0tgwfxfqrjs02wu2q9tny7fc2h936
4
3aa00dc3/84h/1h/0h/0/4
tb1qa2jqhu9ywj2r0ghec00xj6nxdndx22fqqtnwdy
5
3aa00dc3/84h/1h/0h/0/5
tb1qmn454fsvqjg7gytw5xyxf2cc48xag2krjps76w
6
3aa00dc3/84h/1h/0h/0/6
tb1q47e96q9jvl4dm3fmy3j6fwjt8u8mlc2l79dpk6
7
3aa00dc3/84h/1h/0h/0/7
tb1qjgcglx4d4snj4r70l8qxruqjfday4fshraqsed
8
3aa00dc3/84h/1h/0h/0/8
tb1q0kfewchxvyvwtrke35mregekx20keqwkglsh5j
9
3aa00dc3/84h/1h/0h/0/9
tb1q06f6uwt8cnr8t5prkkqv2clnnw4x5ftzdlw0pr
```

Note the following:

* When more than one output is requested, each output is printed in the order requested on a separate line.
* Supplied results, like `address-index`, or intermediate derived results, like `full-address-derivation-path` can also be requested.

### Derive an output descriptor for an account

Output descriptors are described [here](https://github.com/bitcoin/bitcoin/blob/master/doc/descriptors.md) and [here](https://bitcoinops.org/en/topics/output-script-descriptors/). Keytool currently supports `pkh`, `wpkh`, and `sh-wpkh` (`wpkh` nested within `sh`) output descriptor types.

The `address-index` node is set to the wildcard `*` so the returned output descriptor indicates that all address indexes are valid. The asterisk is single-quoted `'*'` to avoid shell globbing.

This example provides a seed, specifies an output descriptor type of `wpkh`, and specifies that any address index is usable. It outputs a descriptor containing the type, the fingerprint of the master key derived from the provided seed, the derivation path of the account key, the account public key, and the derivation path that will be used to derive addresses.

```
$ keytool \
    --seed 1b1454fab426f2729ddcec1b2a6fb539aa2ff0a36d078902994e2fde561d6550 \
    --address-index '*' \
    output-descriptor
wpkh([01577231/84h/0h/0h]xpub6CaE5t6vuSJpRZ4mUb8SX27bmZcinKzUfpn2ZFMYTK81vvrrA9XJZyT8bypiz7gMsjPnpdAJDaQDwvsErnAYi9ZFbDSSSxKR7PtM8msR29D/0/*)
```

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

## Signing Transactions

Keytool can add a signature to a partially-signed Bitcoin transaction (PSBT).

```
keytool \
  --asset btct \
  --psbt "cHNidP8BAMMCAAAAA0DcshmpIS/ZW94f7roto1MhBNhk6Rz/ZaetRRfcedh9AAAAAAD9////wm+mbFIbTYiPlNNhB8P1/vBcwKG/ZryOHMKkL7dSqTYAAAAAAP3///+CmZgUyFrdKNCZUoqfgBhCAFORPdiP/qr0OOt4I5n+CgAAAAAA/f///wJAQg8AAAAAABYAFCBk78yn2RApIqWfuXigrti3WcQRncYQAAAAAAAWABSMyh/Y85oXJKUZQJT+IeAZJk/MeAAAAAAAAQD9PgECAAAABnPYBYJW735g7YMuaFrrK2ZgQj2XELyHukIJmdvOUB4sAAAAAAD+////bmU42jLXSFq3iLHmsyOR/OrF/x0ZQ2lAbecy8CeanlsAAAAAAP7///87WWkGf++EI4Um41R9YUGj00Z0+wvW0fmPYJ+81lWZOAEAAAAA/v///++syYWL4jBkYbhPlOrxJkozfdkL/XJWnhl2nVzVxa2OAAAAAAD+////UAL64CRRErlwBwQM+X5opqpiI0JmpsKs2a1MzGtZh8QAAAAAAP7///8chEjupyVpNkPdCyBE1wpPrS8MGI9EjEGpj7CTgZAXtgAAAAAA/v///wKghgEAAAAAABYAFMzoxnRhxnGxMSMykm4rhwto1dz6vkMPAAAAAAAWABTzi7rfXv1coXc7HvFo2QoeYq4U1E54HAABAR+ghgEAAAAAABYAFMzoxnRhxnGxMSMykm4rhwto1dz6IgYCtKMsZPEI+kb9daMTFeF/+8io6poDvTx+cinpPo9y1EQYDwVpQ1QAAIABAACAAAAAgAAAAAAQAAAAAAEAUgIAAAABbEc0zoXZ/16+PtvwSQ43BytcwpYvO6gGU11xDcaGH7YAAAAAAAEAAAABgEEPAAAAAAAWABQMEbJbdDyzQi5GH1/cPH4QeuPCiwAAAAABAR+AQQ8AAAAAABYAFAwRslt0PLNCLkYfX9w8fhB648KLIgYDcV7jKmCV/4gwmyIZjcVj1LXh1/M4er/DpkPbYiUikt8YDwVpQ1QAAIABAACAAAAAgAAAAACIAAAAAAEAUgIAAAABtvIk4LNoWJem4tfDI5qeVZKR954vLCVJBiqv3pSFR8AAAAAAAAEAAAAB0kEPAAAAAAAWABT93jySCCHYPSeMHxs0jj9i+0UtmAAAAAABAR/SQQ8AAAAAABYAFP3ePJIIIdg9J4wfGzSOP2L7RS2YIgYDhA94BGdguQJPUq61nFoMNrVM4VUaSEJwM7lZcPGyFPUYDwVpQ1QAAIABAACAAAAAgAAAAACGAAAAACICAgFa3FuEwDkszXFDVxN6KU6pVUV/f6d1JSWZ1RsD4CNtGA8FaUNUAACAAQAAgAAAAIAAAAAAiQAAAAAiAgJZ8CLw/Bdus5hyuOPx7cnFfkBkRoTavnS6BRAHi5GOLBgPBWlDVAAAgAEAAIAAAACAAQAAAFIAAAAA" \
  --address-ec-key-wif "cSyLgU8rSvYNU1j6XR2vo5ebSQqza7PR9iFNtkkYrFMwyB5or5gH" \
  psbt-signed
 
cHNidP8BAMMCAAAAA0DcshmpIS/ZW94f7roto1MhBNhk6Rz/ZaetRRfcedh9AAAAAAD9////wm+mbFIbTYiPlNNhB8P1/vBcwKG/ZryOHMKkL7dSqTYAAAAAAP3///+CmZgUyFrdKNCZUoqfgBhCAFORPdiP/qr0OOt4I5n+CgAAAAAA/f///wJAQg8AAAAAABYAFCBk78yn2RApIqWfuXigrti3WcQRncYQAAAAAAAWABSMyh/Y85oXJKUZQJT+IeAZJk/MeAAAAAAAAQD9PgECAAAABnPYBYJW735g7YMuaFrrK2ZgQj2XELyHukIJmdvOUB4sAAAAAAD+////bmU42jLXSFq3iLHmsyOR/OrF/x0ZQ2lAbecy8CeanlsAAAAAAP7///87WWkGf++EI4Um41R9YUGj00Z0+wvW0fmPYJ+81lWZOAEAAAAA/v///++syYWL4jBkYbhPlOrxJkozfdkL/XJWnhl2nVzVxa2OAAAAAAD+////UAL64CRRErlwBwQM+X5opqpiI0JmpsKs2a1MzGtZh8QAAAAAAP7///8chEjupyVpNkPdCyBE1wpPrS8MGI9EjEGpj7CTgZAXtgAAAAAA/v///wKghgEAAAAAABYAFMzoxnRhxnGxMSMykm4rhwto1dz6vkMPAAAAAAAWABTzi7rfXv1coXc7HvFo2QoeYq4U1E54HAABAR+ghgEAAAAAABYAFMzoxnRhxnGxMSMykm4rhwto1dz6IgICtKMsZPEI+kb9daMTFeF/+8io6poDvTx+cinpPo9y1ERIMEUCIQDkmw5j/7puaE9pcUNrOKe4HLpXgP2weErNEsFFJUWQYAIgKAqlZqukvHiRGjZysMu1GAGJRgzZxyjdEan3EZTRrGwBIgYCtKMsZPEI+kb9daMTFeF/+8io6poDvTx+cinpPo9y1EQYDwVpQ1QAAIABAACAAAAAgAAAAAAQAAAAAAEAUgIAAAABbEc0zoXZ/16+PtvwSQ43BytcwpYvO6gGU11xDcaGH7YAAAAAAAEAAAABgEEPAAAAAAAWABQMEbJbdDyzQi5GH1/cPH4QeuPCiwAAAAABAR+AQQ8AAAAAABYAFAwRslt0PLNCLkYfX9w8fhB648KLIgYDcV7jKmCV/4gwmyIZjcVj1LXh1/M4er/DpkPbYiUikt8YDwVpQ1QAAIABAACAAAAAgAAAAACIAAAAAAEAUgIAAAABtvIk4LNoWJem4tfDI5qeVZKR954vLCVJBiqv3pSFR8AAAAAAAAEAAAAB0kEPAAAAAAAWABT93jySCCHYPSeMHxs0jj9i+0UtmAAAAAABAR/SQQ8AAAAAABYAFP3ePJIIIdg9J4wfGzSOP2L7RS2YIgYDhA94BGdguQJPUq61nFoMNrVM4VUaSEJwM7lZcPGyFPUYDwVpQ1QAAIABAACAAAAAgAAAAACGAAAAACICAgFa3FuEwDkszXFDVxN6KU6pVUV/f6d1JSWZ1RsD4CNtGA8FaUNUAACAAQAAgAAAAIAAAAAAiQAAAAAiAgJZ8CLw/Bdus5hyuOPx7cnFfkBkRoTavnS6BRAHi5GOLBgPBWlDVAAAgAEAAIAAAACAAQAAAFIAAAAA
```

Once a transaction is fully signed, Keytool can extract the raw Bitcoin transaction, ready to be placed on the network.

```
keytool \
  --psbt "cHNidP8BAMMCAAAAA0DcshmpIS/ZW94f7roto1MhBNhk6Rz/ZaetRRfcedh9AAAAAAD9////wm+mbFIbTYiPlNNhB8P1/vBcwKG/ZryOHMKkL7dSqTYAAAAAAP3///+CmZgUyFrdKNCZUoqfgBhCAFORPdiP/qr0OOt4I5n+CgAAAAAA/f///wJAQg8AAAAAABYAFCBk78yn2RApIqWfuXigrti3WcQRncYQAAAAAAAWABSMyh/Y85oXJKUZQJT+IeAZJk/MeAAAAAAAAQD9PgECAAAABnPYBYJW735g7YMuaFrrK2ZgQj2XELyHukIJmdvOUB4sAAAAAAD+////bmU42jLXSFq3iLHmsyOR/OrF/x0ZQ2lAbecy8CeanlsAAAAAAP7///87WWkGf++EI4Um41R9YUGj00Z0+wvW0fmPYJ+81lWZOAEAAAAA/v///++syYWL4jBkYbhPlOrxJkozfdkL/XJWnhl2nVzVxa2OAAAAAAD+////UAL64CRRErlwBwQM+X5opqpiI0JmpsKs2a1MzGtZh8QAAAAAAP7///8chEjupyVpNkPdCyBE1wpPrS8MGI9EjEGpj7CTgZAXtgAAAAAA/v///wKghgEAAAAAABYAFMzoxnRhxnGxMSMykm4rhwto1dz6vkMPAAAAAAAWABTzi7rfXv1coXc7HvFo2QoeYq4U1E54HAABAR+ghgEAAAAAABYAFMzoxnRhxnGxMSMykm4rhwto1dz6IgICtKMsZPEI+kb9daMTFeF/+8io6poDvTx+cinpPo9y1ERIMEUCIQDkmw5j/7puaE9pcUNrOKe4HLpXgP2weErNEsFFJUWQYAIgKAqlZqukvHiRGjZysMu1GAGJRgzZxyjdEan3EZTRrGwBIgYCtKMsZPEI+kb9daMTFeF/+8io6poDvTx+cinpPo9y1EQYDwVpQ1QAAIABAACAAAAAgAAAAAAQAAAAAAEAUgIAAAABbEc0zoXZ/16+PtvwSQ43BytcwpYvO6gGU11xDcaGH7YAAAAAAAEAAAABgEEPAAAAAAAWABQMEbJbdDyzQi5GH1/cPH4QeuPCiwAAAAABAR+AQQ8AAAAAABYAFAwRslt0PLNCLkYfX9w8fhB648KLIgIDcV7jKmCV/4gwmyIZjcVj1LXh1/M4er/DpkPbYiUikt9IMEUCIQDeFzLuE1DRcQEvwJevA+kf8m8Ay9LJbnoP4qarthudHQIgYyk2ZQEIbvXSFDh37tTG9u2T247SbCfzYNsbLFW4AVYBIgYDcV7jKmCV/4gwmyIZjcVj1LXh1/M4er/DpkPbYiUikt8YDwVpQ1QAAIABAACAAAAAgAAAAACIAAAAAAEAUgIAAAABtvIk4LNoWJem4tfDI5qeVZKR954vLCVJBiqv3pSFR8AAAAAAAAEAAAAB0kEPAAAAAAAWABT93jySCCHYPSeMHxs0jj9i+0UtmAAAAAABAR/SQQ8AAAAAABYAFP3ePJIIIdg9J4wfGzSOP2L7RS2YIgIDhA94BGdguQJPUq61nFoMNrVM4VUaSEJwM7lZcPGyFPVHMEQCIFALHbiTldYsUElBStxS550LVScYBmmsgRrWGWPVfr7EAiAA1kGB/iscnQrbJnKtBhLEDE4MsCfiLjQNNakjB+9MjgEiBgOED3gEZ2C5Ak9SrrWcWgw2tUzhVRpIQnAzuVlw8bIU9RgPBWlDVAAAgAEAAIAAAACAAAAAAIYAAAAAIgICAVrcW4TAOSzNcUNXE3opTqlVRX9/p3UlJZnVGwPgI20YDwVpQ1QAAIABAACAAAAAgAAAAACJAAAAACICAlnwIvD8F26zmHK44/HtycV+QGRGhNq+dLoFEAeLkY4sGA8FaUNUAACAAQAAgAAAAIABAAAAUgAAAAA=" \
  transaction

0200000000010340dcb219a9212fd95bde1feeba2da3532104d864e91cff65a7ad4517dc79d87d0000000000fdffffffc26fa66c521b4d888f94d36107c3f5fef05cc0a1bf66bc8e1cc2a42fb752a9360000000000fdffffff82999814c85add28d099528a9f8018420053913dd88ffeaaf438eb782399fe0a0000000000fdffffff0240420f00000000001600142064efcca7d9102922a59fb978a0aed8b759c4119dc61000000000001600148cca1fd8f39a1724a5194094fe21e019264fcc7802483045022100e49b0e63ffba6e684f6971436b38a7b81cba5780fdb0784acd12c145254590600220280aa566aba4bc78911a3672b0cbb5180189460cd9c728dd11a9f71194d1ac6c012102b4a32c64f108fa46fd75a31315e17ffbc8a8ea9a03bd3c7e7229e93e8f72d44402483045022100de1732ee1350d171012fc097af03e91ff26f00cbd2c96e7a0fe2a6abb61b9d1d02206329366501086ef5d2143877eed4c6f6ed93db8ed26c27f360db1b2c55b80156012103715ee32a6095ff88309b22198dc563d4b5e1d7f3387abfc3a643db62252292df024730440220500b1db89395d62c5049414adc52e79d0b5527180669ac811ad61963d57ebec4022000d64181fe2b1c9d0adb2672ad0612c40c4e0cb027e22e340d35a92307ef4c8e012103840f78046760b9024f52aeb59c5a0c36b54ce1551a48427033b95970f1b214f500000000
```

Keytool can be invoked repeatedly to add multiple signatures to a PSBT and then extract the final transaction, with the progessively-signed PSBT handed from one invocation to the next using the Unix pipe operator (`|`)

```
PSBT_TO_SIGN=cHNidP8BAMMCAAAAA0DcshmpIS/ZW94f7roto1MhBNhk6Rz/ZaetRRfcedh9AAAAAAD9////wm+mbFIbTYiPlNNhB8P1/vBcwKG/ZryOHMKkL7dSqTYAAAAAAP3///+CmZgUyFrdKNCZUoqfgBhCAFORPdiP/qr0OOt4I5n+CgAAAAAA/f///wJAQg8AAAAAABYAFCBk78yn2RApIqWfuXigrti3WcQRncYQAAAAAAAWABSMyh/Y85oXJKUZQJT+IeAZJk/MeAAAAAAAAQD9PgECAAAABnPYBYJW735g7YMuaFrrK2ZgQj2XELyHukIJmdvOUB4sAAAAAAD+////bmU42jLXSFq3iLHmsyOR/OrF/x0ZQ2lAbecy8CeanlsAAAAAAP7///87WWkGf++EI4Um41R9YUGj00Z0+wvW0fmPYJ+81lWZOAEAAAAA/v///++syYWL4jBkYbhPlOrxJkozfdkL/XJWnhl2nVzVxa2OAAAAAAD+////UAL64CRRErlwBwQM+X5opqpiI0JmpsKs2a1MzGtZh8QAAAAAAP7///8chEjupyVpNkPdCyBE1wpPrS8MGI9EjEGpj7CTgZAXtgAAAAAA/v///wKghgEAAAAAABYAFMzoxnRhxnGxMSMykm4rhwto1dz6vkMPAAAAAAAWABTzi7rfXv1coXc7HvFo2QoeYq4U1E54HAABAR+ghgEAAAAAABYAFMzoxnRhxnGxMSMykm4rhwto1dz6IgYCtKMsZPEI+kb9daMTFeF/+8io6poDvTx+cinpPo9y1EQYDwVpQ1QAAIABAACAAAAAgAAAAAAQAAAAAAEAUgIAAAABbEc0zoXZ/16+PtvwSQ43BytcwpYvO6gGU11xDcaGH7YAAAAAAAEAAAABgEEPAAAAAAAWABQMEbJbdDyzQi5GH1/cPH4QeuPCiwAAAAABAR+AQQ8AAAAAABYAFAwRslt0PLNCLkYfX9w8fhB648KLIgYDcV7jKmCV/4gwmyIZjcVj1LXh1/M4er/DpkPbYiUikt8YDwVpQ1QAAIABAACAAAAAgAAAAACIAAAAAAEAUgIAAAABtvIk4LNoWJem4tfDI5qeVZKR954vLCVJBiqv3pSFR8AAAAAAAAEAAAAB0kEPAAAAAAAWABT93jySCCHYPSeMHxs0jj9i+0UtmAAAAAABAR/SQQ8AAAAAABYAFP3ePJIIIdg9J4wfGzSOP2L7RS2YIgYDhA94BGdguQJPUq61nFoMNrVM4VUaSEJwM7lZcPGyFPUYDwVpQ1QAAIABAACAAAAAgAAAAACGAAAAACICAgFa3FuEwDkszXFDVxN6KU6pVUV/f6d1JSWZ1RsD4CNtGA8FaUNUAACAAQAAgAAAAIAAAAAAiQAAAAAiAgJZ8CLw/Bdus5hyuOPx7cnFfkBkRoTavnS6BRAHi5GOLBgPBWlDVAAAgAEAAIAAAACAAQAAAFIAAAAA
KEY_1=cSyLgU8rSvYNU1j6XR2vo5ebSQqza7PR9iFNtkkYrFMwyB5or5gH
KEY_2=cVok3VJ1fGhdT2yAndKHFWvgTbDsdVFXb8xE9rsAcQASuS4JV9HG
KEY_3=cMaqfcb16JjrCdfYTfT2vRKFpGjdAugAucEAxw3VuhN91XgjKqzF
keytool --asset btct --psbt ${PSBT_TO_SIGN} --address-ec-key-wif ${KEY_1} psbt-signed \
  | keytool --asset btct --psbt '' --address-ec-key-wif ${KEY_2} psbt-signed \
  | keytool --asset btct --psbt '' --address-ec-key-wif ${KEY_3} psbt-signed \
  | keytool --psbt '' transaction
  
0200000000010340dcb219a9212fd95bde1feeba2da3532104d864e91cff65a7ad4517dc79d87d0000000000fdffffffc26fa66c521b4d888f94d36107c3f5fef05cc0a1bf66bc8e1cc2a42fb752a9360000000000fdffffff82999814c85add28d099528a9f8018420053913dd88ffeaaf438eb782399fe0a0000000000fdffffff0240420f00000000001600142064efcca7d9102922a59fb978a0aed8b759c4119dc61000000000001600148cca1fd8f39a1724a5194094fe21e019264fcc7802483045022100e49b0e63ffba6e684f6971436b38a7b81cba5780fdb0784acd12c145254590600220280aa566aba4bc78911a3672b0cbb5180189460cd9c728dd11a9f71194d1ac6c012102b4a32c64f108fa46fd75a31315e17ffbc8a8ea9a03bd3c7e7229e93e8f72d44402483045022100de1732ee1350d171012fc097af03e91ff26f00cbd2c96e7a0fe2a6abb61b9d1d02206329366501086ef5d2143877eed4c6f6ed93db8ed26c27f360db1b2c55b80156012103715ee32a6095ff88309b22198dc563d4b5e1d7f3387abfc3a643db62252292df024730440220500b1db89395d62c5049414adc52e79d0b5527180669ac811ad61963d57ebec4022000d64181fe2b1c9d0adb2672ad0612c40c4e0cb027e22e340d35a92307ef4c8e012103840f78046760b9024f52aeb59c5a0c36b54ce1551a48427033b95970f1b214f500000000

```

## Version History

### 0.3.0, 11/2/2020

* Added nodes for deriving segwit addresses, signing PSBTs, and extracting raw transactions from PSBTs.

### 0.2.1, 10/22/2020

* Renamed `output-descriptor-type` to `output-type`.
* Made `purpose` dependent on `output-type`.
* Changed default `output-type` to `wpkh`.

### 0.2.0, 10/22/2020

* Support for `output-descriptor`.

### 0.1.0, 9/28/2020

* First testing release.
