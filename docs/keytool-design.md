# Keytool

**© 2020 Blockchain Commons**

Authors: Wolf McNally, Christopher Allen

---

Keytool is a CLI tool that implements a data flow graph for deriving cryptocurrency keys and addresses:

![Derivations](https://user-images.githubusercontent.com/676176/88364937-50c2e000-cd39-11ea-845b-7f5c0ab3c44c.png)

Each node in the graph is a specific piece of data that can be supplied directly on the command line, or derived from its predecessors. Two or more arrows entering a box indicate more than one possible set of inputs can be used to derive it. AND junctors indicate that *all* predecessor attributes must be supplied.

As its inputs, Keytool can be supplied with any of the nodes, and as its outputs Keytool can be asked for any of the existing or derivable nodes. In this way, Keytool can implement a very short derivation such as "Derive a master HD key from a seed," or a long sequence of derivations from a seed all the way to a payment address, also outputting any of the intermediate steps along the way.

## Usage

```
$ keytool
	[--seed HEX]
	[--asset btc|btct] # default btc
	[--purpose INT] # default 44
	[--master-key XPRV]
	[--account-index INDEX] # default 0
	[--chain-type internal|external] # default external
	[--address-index INDEX] # default 0
	[--address-count INT_RANGE] # default 0..0
	[--account-derivation-path BIP32_PATH]
	[--account-key XPRV]
	[--address-derivation-path BIP32_PATH]
	[--account-pub-key XPUB]
	[--address-key XPRV]
	[--password STRING] # default none
	[--address-pub-key XPUB]
	[--address-ec-key WIF]
	[--address-pub-ec-key HEX]
	[--address BASE58_CHECK]

	output-attributes...
```	

## Examples

Derive master HD key from seed:

```
$ keytool --seed 1eb5338edac6fae54cbb172091ae6c1a --asset btc \
	master-key
xprv9s21ZrQH143K4FAunaSG9eYwrAaaChpSYwYF22eJiZJrz5zSBKTg7NJcFqWR2UZc7EhneSMJhHLmPsKx96UDgv9CdoLc6JfQo3AncKhYNSc
```

Derive master HD key from seed, providing the inputs via STDIN:

* Note that inputs specified without parameters will be expected in the order they are specified via STDIN.

```
$ keytool --seed --asset master-key
1eb5338edac6fae54cbb172091ae6c1a
btc^D
xprv9s21ZrQH143K4FAunaSG9eYwrAaaChpSYwYF22eJiZJrz5zSBKTg7NJcFqWR2UZc7EhneSMJhHLmPsKx96UDgv9CdoLc6JfQo3AncKhYNSc
```

Derive account #100 private and public keys from master HD key:

```
$ keytool --master-key xprv9s21ZrQH143K4FAunaSG9eYwrAaaChpSYwYF22eJiZJrz5zSBKTg7NJcFqWR2UZc7EhneSMJhHLmPsKx96UDgv9CdoLc6JfQo3AncKhYNSc \
	--account-index 100 \
	account-key \
	account-pub-key
xprv9zQ4qWYWfk6abm8HnBh1T4BFAPdqY4T6Re28wGF3QV89dUDfCEnuDbj2KGBx5PmwGLA9gQjBz132Rx6eK6Fchtg5Sz3nytLoRdH92X4xToY
xpub6DPRF25QW7espFCktDE1pC7yiRUKwXAwnrwjjeeexpf8WGYojn79mQ3WAXimYrFp5az72QrFD3DFtpVFeiTXy9k1WWYz6mGnEvMhKRN3tne
```

Derive address #1234 of account #0 private key from master HD key:

```
$ keytool --master-key xprv9s21ZrQH143K4FAunaSG9eYwrAaaChpSYwYF22eJiZJrz5zSBKTg7NJcFqWR2UZc7EhneSMJhHLmPsKx96UDgv9CdoLc6JfQo3AncKhYNSc \
	--address-index 1234 \
	address-private-key
xprvA1hVu3qNBYbrEUC2omdQ6STcfi4Jroq3PUN8yfChGUgrzvoCBeaqyj1GjGfD2qpdDjMC9K3NBwWmB5BifSxkiEkraiAU67LijjsY7xEeh25
```

Derive an arbitrary BIP32 private and public keys from master HD key.

* Note: Since `'` is a UNIX command-line quote mark, BIP32 paths given to Keytool can use either `'` or `h` to denote hardened derivations on input.

```
$ keytool --master-key xprv9s21ZrQH143K4FAunaSG9eYwrAaaChpSYwYF22eJiZJrz5zSBKTg7NJcFqWR2UZc7EhneSMJhHLmPsKx96UDgv9CdoLc6JfQo3AncKhYNSc \
	--address-derivation-path m/99h/1h/2h/3 \
	address-key \
	address-pub-key
xprv9zvzy7RBcHUqSY5uPpz5jMgA4f98yCxW21Qrwg5qQu3GcqTSBsAgpRXfTnmE4SqSbLJVHx4NdWFEc1CzwVps15ynZtFPEtpaNh9uDkd3auw
xpub6DvMNcx5Sf38f2ANVrX66VctcgydNfgMPELTk4VSyEaFVdnajQUwNDr9K54VQ2SRM7gmcQmJcMa23xyWEwU2ehdU9tgFBozSUnLaWJAhmBk
```

Derive 10 successive bitcoin addreses from a seed.

Note the following: 

* When more than one output is requested, each output is printed in the order requested on a separate line.
* `address-index` can take a range of indexes, causing the derivations that depend on it to be repeated once for each index.
* Supplied results, like `address-index`, or intermediate derived results, like `address-derivation-path` can also be requested.

```
$ keytool --seed 1eb5338edac6fae54cbb172091ae6c1a --asset btc --address-index 1..10 \
	address-index \
	address-derivation-path \
	address
1
m/44'/0'/0'/0/1
1J3DmbvZLACBkXxFpehuBDMziYmpRmM1cs
2
m/44'/0'/0'/0/2
1CbgRLCWsgRs92gfcZRcdu49AczLSnhE8v
3
m/44'/0'/0'/0/3
16qW5iyYtvB9ZB3hUjQBW9q99DYdqAALvc
4
m/44'/0'/0'/0/4
1FK9hTgVqAXh6Vgnf8p4t7UnzkdfKjH1T7
5
m/44'/0'/0'/0/5
1DG8MXbTgRPpjpKBw3ySwBh6mq2K9rPD8d
6
m/44'/0'/0'/0/6
16GB39n2RK3KF95nxdVCMM3PSeAXiB7hUu
7
m/44'/0'/0'/0/7
1JSyFfXprnofwZhB7X2DxN3WfQtk1Kibxs
8
m/44'/0'/0'/0/8
14mkCprpUs86Q8eUPQVwadhfNGpMcSPP67
9
m/44'/0'/0'/0/9
13LFSpGpeYjnFdbT4Sw8qHhCMKACAyoapR
10
m/44'/0'/0'/0/10
1FuyCXFj3M99yh4v1YiaKXatfBQ5QysX6N
```