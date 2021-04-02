# 🔑 Keytool

**Version 0.5.0**<br/>**March 29, 2021**

*Copyright © 2020 by Blockchain Commons, LLC*<br/>*Licensed under the "BSD-2-Clause Plus Patent License"*

---

Keytool is a CLI tool that implements a data flow graph for deriving cryptocurrency keys and addresses, and signing transactions.

As its inputs, Keytool can be supplied with any of the attributes for the nodes, and as its outputs Keytool can be asked for any of the attributes of the existing or derivable nodes. In this way, Keytool can implement a very short derivation such as "Derive a master HD key from a seed," or a long sequence of derivations from a seed all the way to a payment address, also outputting any of the intermediate steps along the way.

## Derivations

These derivations are expressed visually in the graph below. Each node in the graph is a specific attribute that can be supplied directly on the command line, or derived from its predecessors. Two or more arrows entering a node indicate more than one possible set of inputs can be used to derive it. AND junctors indicate that *all* predecessor attributes must be supplied, unless a predecessor is marked `optional`.

![](docs/Derivations.jpg)

Below is a list of the single-step derivations that Keytool can perform, in the form `A <- [B, C, ...]` where `A` is the derived attributed and `[B, C, ...]` are the attributes required to derive it.

If an attribute is listed more than once, then it has more than one derivation path. If the first derivation path cannot be used, the second one will be tried, and so on.

All the attributes except `seed` are either derivable from other attributes or have a default value. The default values are suitable for use with Bitcoin mainnet. If these attributes are supplied on the command line, they override the defaults.

```
account-derivation-path <- [master-key-fingerprint, purpose, coin-type, account-index]
account-index (default: 0)
account-key <- [account-key-base58]
account-key <- [master-key, account-derivation-path]
account-key-base58 <- [account-key]
account-pub-key <- [account-key]
account-pub-key <- [account-pub-key-base58]
account-pub-key-base58 <- [account-pub-key]
address-derivation-path <- [chain-type-int, address-index]
address-ec-key <- [address-ec-key-wif, network]
address-ec-key <- [address-key]
address-ec-key-wif <- [address-ec-key, network]
address-index (default: 0)
address-key <- [account-key, address-derivation-path, is-derivable]
address-key <- [address-key-base58]
address-key <- [master-key, full-address-derivation-path, is-derivable]
address-key-base58 <- [address-key]
address-pkh <- [address-pub-ec-key, asset, network]
address-pub-ec-key <- [address-ec-key]
address-pub-ec-key <- [address-pub-key]
address-pub-key <- [account-pub-key, address-derivation-path, is-derivable]
address-pub-key <- [address-key]
address-pub-key <- [address-pub-key-base58]
address-pub-key-base58 <- [address-pub-key]
address-segwit <- [address-pub-key]
address-sh <- [address-pub-ec-key, asset, network]
asset (default: btc)
chain-type (default: external)
chain-type-int <- [chain-type]
coin-type <- [asset, network]
derived-key <- [key-response]
derived-key-base58 <- [derived-key]
full-address-derivation-path <- [account-derivation-path, address-derivation-path]
is-derivable <- [source-key]
is-derivable (default: true)
key-request <- [key-request-derivation-path, key-request-type, key-request-id, key-request-description, asset, network, is-derivable]
key-request-derivation-path <- [full-address-derivation-path]
key-request-derivation-path <- [key-request]
key-request-description (default: empty)
key-request-description <- [key-request]
key-request-id (default: unique)
key-request-id <- [key-request]
key-request-id <- [key-response]
key-request-type (default: private)
key-request-type <- [key-request]
key-response <- [key-request, source-key]
master-key <- [master-key-base58]
master-key <- [seed, asset, network]
master-key-base58 <- [master-key]
master-key-fingerprint <- [master-key]
network (default: mainnet)
output-descriptor <- [output-type, account-derivation-path, address-derivation-path, account-pub-key]
output-type (default: wpkh)
psbt-base64 <- [psbt]
psbt-finalized <- [psbt]
psbt-finalized-base64 <- [psbt-finalized]
psbt-finalized-hex <- [psbt-finalized]
psbt-hex <- [psbt]
psbt-signed <- [psbt, address-ec-key]
psbt-signed-base64 <- [psbt-signed]
psbt-signed-hex <- [psbt-signed]
purpose <- [output-type]
seed <- [seed-hex, seed-name (optional), seed-note (optional)]
seed-digest <- [seed-request]
seed-digest <- [seed]
seed-hex <- [seed]
seed-name <- [seed]
seed-note <- [seed]
seed-request <- [seed-digest seed-request-id seed-request-description]
seed-request-description (default: empty)
seed-request-description <- [seed-request]
seed-request-id (default: unique)
seed-request-id <- [seed-request]
seed-response <- [seed-request, seed]
source-key <- [master-key]
transaction <- [psbt-finalized]
transaction-hex <- [transaction]
```

## Formats

Keytool uses particular formats for input and output attributes:

|Type | Description |
|:---|:---|
| ADDRESS | An address in Base58 check format.
| BASE58 | [An HD private key starting with `xprv`, `xpub` or similar.](https://github.com/bitcoin/bips/blob/master/bip-0032.mediawiki)
| BASE64 | Data in Base64 format.
| BIP32_PATH | A series of BIP-32 path elements, possibly starting with the source fingerprint as 8 hex digits and followed by integers, which may optionally be followed by `h` (for hardened derivation). Example: `1a2b3c4d/44h/0h/0h/0/123`.
| BOOLEAN | `true` or `false`.
| ECPRV | An elliptic curve private key in hex format.
| ECPUB | A compressed elliptic curve public key in hex format.
| ENUM | A specific, value from a set of enumerated values. For example `network` can have the value `mainnet` or `testnet`.
| HDKEY | [A BIP-32 HD key.](https://github.com/BlockchainCommons/Research/blob/master/papers/bcr-2020-007-hdkey.md)
| HEX | A string of hexadecimal digits, case-insensitive.
| INDEX | A non-negative integer.
| INDEX_BOUND | An INDEX or the wildcard `*`. Note that on the Unix command line, you will need to enclose `*` in single quotes (`'*'`) to avoid shell globbing.
| OUTPUT_DESCRIPTOR | [A single-signature output descriptor.](https://github.com/bitcoin/bitcoin/blob/master/doc/descriptors.md)
| PSBT | [A partially-signed Bitcoin transaction (PSBT).](https://github.com/bitcoin/bips/blob/master/bip-0174.mediawiki)
| REQUEST | [A request for a seed, derived HD key, or other service.](https://github.com/BlockchainCommons/Research/blob/master/papers/bcr-2021-001-request.md)
| RESPONSE | [The response to a corresponding request.](https://github.com/BlockchainCommons/Research/blob/master/papers/bcr-2021-001-request.md)
| TEXT | A text annotation.
| TRANSACTION | A raw Bitcoin transaction.
| UUID | [A Universally Unique Identifier.](https://en.wikipedia.org/wiki/Universally_unique_identifier)
| WIF | [An elliptic curve private key in Wallet Import Format.](https://en.bitcoin.it/wiki/Wallet_import_format)

The Uniform Resource (UR) format is described [here](https://github.com/BlockchainCommons/Research/blob/master/papers/bcr-2020-005-ur.md).

## Converting Formats

Nodes that hold the types below accept input in Uniform Resource (UR) format and others as well. They output their contents in UR format, but other nodes may derive from them that provide output in other formats.

|Type | Primary type | Other accepted input types | Nodes for output formats
|:---|:---|:---|:---
SEED | ur:crypto-seed | hex | hex
HDKEY | ur:crypto-hdkey | Base58 | Base58
PSBT | ur:crypto-psbt | Hex, Base64 | Hex, Base64
REQUEST | ur:crypto-request
RESPONSE | ur:crypto-response
TRANSACTION | ur:crypto-tx | Hex | Hex

## Usage

```
Usage: keytool [-?V] [--account-derivation-path=BIP32_PATH]
            [--account-index=INDEX] [--account-key=HDKEY]
            [--account-key-base58=BASE58] [--account-pub-key=HDKEY]
            [--account-pub-key-base58=BASE58]
            [--address-derivation-path=BIP32_PATH] [--address-ec-key=ECPRV]
            [--address-ec-key-wif=WIF] [--address-index=INDEX_BOUND]
            [--address-key=HDKEY] [--address-key-base58=BASE58]
            [--address-pkh=ADDRESS] [--address-pub-ec-key=ECPUB]
            [--address-pub-key=HDKEY] [--address-pub-key-base58=BASE58]
            [--address-segwit=ADDRESS] [--address-sh=ADDRESS] [--asset=ENUM btc
            | eth] [--chain-type=ENUM internal | external | identity]
            [--chain-type-int=INDEX] [--coin-type=INDEX] [--derived-key=HDKEY]
            [--full-address-derivation-path=BIP32_PATH]
            [--key-request=REQUEST] [--key-request-derivation-path=BIP32_PATH]
            [--key-request-description=TEXT] [--key-request-id=UUID]
            [--key-request-type=ENUM private | public]
            [--key-response=RESPONSE] [--master-key=HDKEY]
            [--master-key-base58=BASE58] [--master-key-fingerprint=HEX]
            [--network=ENUM mainnet | testnet]
            [--output-descriptor=OUTPUT_DESCRIPTOR] [--output-type=ENUM wpkh |
            pkh | sh-wpkh] [--psbt=PSBT] [--psbt-base64=BASE64]
            [--psbt-finalized=PSBT] [--psbt-finalized-base64=BASE64]
            [--psbt-finalized-hex=HEX] [--psbt-hex=HEX] [--psbt-signed=PSBT]
            [--psbt-signed-base64=BASE64] [--psbt-signed-hex=HEX]
            [--purpose=INDEX] [--seed=SEED] [--seed-digest=HEX]
            [--seed-hex=HEX] [--seed-name=TEXT] [--seed-note=TEXT]
            [--seed-request=REQUEST] [--seed-request-description=TEXT]
            [--seed-request-id=UUID] [--seed-response=RESPONSE]
            [--source-key=HDKEY] [--transaction=TRANSACTION]
            [--transaction-hex=HEX] [--help] [--usage] [--version]
  
  output-attributes...
```

## Example Usages

### Derive a master HD key from seed and display it in both UR and Base58 formats.

```
$ keytool \
	--seed 1eb5338edac6fae54cbb172091ae6c1a \
	master-key \
	master-key-base58

ur:crypto-hdkey/oxadykaoykaxhdclaeqzwzssbnzczmkneccestferochaeryiheycpynwllertqztasseskslkidjofelfaahdcxuynskidktswzeejshsdldsfykbtloeemfhgluttycxvaennykenbghhfuyampddidavsjpze
xprv9s21ZrQH143K4FAunaSG9eYwrAaaChpSYwYF22eJiZJrz5zSBKTg7NJcFqWR2UZc7EhneSMJhHLmPsKx96UDgv9CdoLc6JfQo3AncKhYNSc
```

### Derive address #1234 of account #0 private key from master HD key

```
$ keytool \
	--master-key ur:crypto-hdkey/oxadykaoykaxhdclaeqzwzssbnzczmkneccestferochaeryiheycpynwllertqztasseskslkidjofelfaahdcxuynskidktswzeejshsdldsfykbtloeemfhgluttycxvaennykenbghhfuyampddidavsjpze \
	--address-index 1234 \
	address-key

ur:crypto-hdkey/onaoykaxhdclaelthegdiskssarfbatlpeenayuofghsoeenvegmqzmstpgtwksbcywspkroleaaztaahdcxmevdctknhdaonyeypllfmezeksnllgieolyabnfnonfdftidndmtbkwefmfpecbsamoeadlecsghykaeykaeykaewkcfaatdwkaocyselpnldsaycyghsbyngdvojzlywm
```

### Derive an arbitrary BIP32 private and public keys from master HD key

```
$ keytool \
    --master-key xprv9s21ZrQH143K4FAunaSG9eYwrAaaChpSYwYF22eJiZJrz5zSBKTg7NJcFqWR2UZc7EhneSMJhHLmPsKx96UDgv9CdoLc6JfQo3AncKhYNSc \
    --full-address-derivation-path m/99h/1h/2h/3 \
    address-key-base58 \
    address-pub-key-base58

xprv9zvzy7RBcHUqSY5uPpz5jMgA4f98yCxW21Qrwg5qQu3GcqTSBsAgpRXfTnmE4SqSbLJVHx4NdWFEc1CzwVps15ynZtFPEtpaNh9uDkd3auw
xpub6DvMNcx5Sf38f2ANVrX66VctcgydNfgMPELTk4VSyEaFVdnajQUwNDr9K54VQ2SRM7gmcQmJcMa23xyWEwU2ehdU9tgFBozSUnLaWJAhmBk
```

* Note: Since `'` is a UNIX command-line quote mark, BIP32 paths given to Keytool use `h` to denote hardened derivations.

### Derive 10 successive bitcoin addresses from a seed

```
for (( i=0; i <= 9; ++i ))
do
	keytool \
		--network testnet \
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
CLI:    $ keytool --seed '' master-key-base58
STDIN:  8935a8068526d84da555cdb741a3b8a8
STDOUT: xprv9s21ZrQH143K3rJgfaUmUCueBxbDT1bbksxayh7ik1nZN41zAy2kQNop8REXEAAwqwes1E8rxk1313tyzsNmMq69h4vDyyrXkmjDZ2Nf3pN
```

You can of course use shell argument replacement with Seedtool to generate a seed, and from that a master key in one line:

```
$ keytool --seed `seedtool` master-key
ur:crypto-hdkey/oxadykaoykaxhdclaedynbntlglkzctoueutbenynnwyhhcnfxjotnosbwcpstbdveehiolplstbaxdnesaahdcxhncnjevwcaltahcsamlakptlehattipejswksnrfhfleadcmamhkndmndkledttpjtfpbbme
```

Because you can provide inputs on STDIN, you can also use the shell to pipe the result of Seedtool to Keytool:

```
$ seedtool | keytool --seed '' master-key
ur:crypto-hdkey/oxadykaoykaxhdclaeplrliolfpldipdkkatttmslfmswfrftpnljzftrhyndrylknlahfgwwkcnkiayuraahdcxbawegrdtdldmtlfxytkbdkvyfxehztvokowypdlfayuyctchzosamdmtptdpteuroxfrpdyk
```

You can use Keytool to print out both the seed generated by Seedtool and the master key:

```
$ seedtool | keytool --seed '' seed master-key
ur:crypto-seed/oyadgdeeaefdttoejnsptegmmwflcscxtpuoswgslycpin
ur:crypto-hdkey/oxadykaoykaxhdclaemeptlffhinplayjocwkokbnemozsfmselunlmhioqdhhrlfzjypmcywzfdzmleghaahdcxayjkasgtdlpasroeotltpreohyiejkkpwdldcecnterftpurcphkgodrhhcmdagyknfydidy
```

Other scripts can provide all the necessary inputs to keytool:

```
$ { seedtool && echo 'testnet'; } | keytool --seed '' --network '' master-key
ur:crypto-hdkey/onadykaoykaxhdclaepehscecabainlaytsffhtddsktonvytdrsynytsbtpoyoxpsytmnieonrtcwkogtaahdcxlsvtswoewpghwebgadldrspynepmnytybtprylpkwdvyzmuebtlfsnjeynmupyskahoyaoadehiakklf
```

## Signing Transactions

Keytool can add a signature to a partially-signed Bitcoin transaction (PSBT).

```
keytool \
  --network testnet \
  --psbt "cHNidP8BAMMCAAAAA0DcshmpIS/ZW94f7roto1MhBNhk6Rz/ZaetRRfcedh9AAAAAAD9////wm+mbFIbTYiPlNNhB8P1/vBcwKG/ZryOHMKkL7dSqTYAAAAAAP3///+CmZgUyFrdKNCZUoqfgBhCAFORPdiP/qr0OOt4I5n+CgAAAAAA/f///wJAQg8AAAAAABYAFCBk78yn2RApIqWfuXigrti3WcQRncYQAAAAAAAWABSMyh/Y85oXJKUZQJT+IeAZJk/MeAAAAAAAAQD9PgECAAAABnPYBYJW735g7YMuaFrrK2ZgQj2XELyHukIJmdvOUB4sAAAAAAD+////bmU42jLXSFq3iLHmsyOR/OrF/x0ZQ2lAbecy8CeanlsAAAAAAP7///87WWkGf++EI4Um41R9YUGj00Z0+wvW0fmPYJ+81lWZOAEAAAAA/v///++syYWL4jBkYbhPlOrxJkozfdkL/XJWnhl2nVzVxa2OAAAAAAD+////UAL64CRRErlwBwQM+X5opqpiI0JmpsKs2a1MzGtZh8QAAAAAAP7///8chEjupyVpNkPdCyBE1wpPrS8MGI9EjEGpj7CTgZAXtgAAAAAA/v///wKghgEAAAAAABYAFMzoxnRhxnGxMSMykm4rhwto1dz6vkMPAAAAAAAWABTzi7rfXv1coXc7HvFo2QoeYq4U1E54HAABAR+ghgEAAAAAABYAFMzoxnRhxnGxMSMykm4rhwto1dz6IgYCtKMsZPEI+kb9daMTFeF/+8io6poDvTx+cinpPo9y1EQYDwVpQ1QAAIABAACAAAAAgAAAAAAQAAAAAAEAUgIAAAABbEc0zoXZ/16+PtvwSQ43BytcwpYvO6gGU11xDcaGH7YAAAAAAAEAAAABgEEPAAAAAAAWABQMEbJbdDyzQi5GH1/cPH4QeuPCiwAAAAABAR+AQQ8AAAAAABYAFAwRslt0PLNCLkYfX9w8fhB648KLIgYDcV7jKmCV/4gwmyIZjcVj1LXh1/M4er/DpkPbYiUikt8YDwVpQ1QAAIABAACAAAAAgAAAAACIAAAAAAEAUgIAAAABtvIk4LNoWJem4tfDI5qeVZKR954vLCVJBiqv3pSFR8AAAAAAAAEAAAAB0kEPAAAAAAAWABT93jySCCHYPSeMHxs0jj9i+0UtmAAAAAABAR/SQQ8AAAAAABYAFP3ePJIIIdg9J4wfGzSOP2L7RS2YIgYDhA94BGdguQJPUq61nFoMNrVM4VUaSEJwM7lZcPGyFPUYDwVpQ1QAAIABAACAAAAAgAAAAACGAAAAACICAgFa3FuEwDkszXFDVxN6KU6pVUV/f6d1JSWZ1RsD4CNtGA8FaUNUAACAAQAAgAAAAIAAAAAAiQAAAAAiAgJZ8CLw/Bdus5hyuOPx7cnFfkBkRoTavnS6BRAHi5GOLBgPBWlDVAAAgAEAAIAAAACAAQAAAFIAAAAA" \
  --address-ec-key-wif "cSyLgU8rSvYNU1j6XR2vo5ebSQqza7PR9iFNtkkYrFMwyB5or5gH" \
  psbt-signed
 
ur:crypto-psbt/hkaarfjojkidjyzmadaesraoaeaeaeaxfzuoprcfptcldltahpuectwyrddpotguclaatpiewlcezmihospmfechuokktpkiaeaeaeaeaezczmzmzmsajloljzgmcwgtlomymwtehsatsrykzewthhrtoyrsiyrfmncesaoxdlrlgmptenaeaeaeaeaezczmzmzmlfnlmkbbsphtutdetinlgmlenelacsfwaegumefstpmyzepkwketwmkscnnlzebkaeaeaeaeaezczmzmzmaofzfwbsaeaeaeaeaecmaebbcxiewssfostabedtcponnerhksnbpltprlhkssbyntswbeaeaeaeaeaecmaebblksgcttpwfnychdkoncffzmwzeclvtcfdsgwsfksaeaeaeaeaeadaezcfmadaoaeaeaeamjktpahlfhfwskbhnwelsdmishtwmdniyhnfwfsmsberfltrdfwasnluytogdckdwaeaeaeaeaezezmzmzmjtihettneytsfdhtrllopavaqdcnmeztwdskzmcacffxinfzjnvdeywtdinynnhpaeaeaeaeaezezmzmzmfrhkinamlbwslrcnlpdsvlghkihsfpottefgjyzobdtbttytmyhnnerftbgonletadaeaeaeaezezmzmzmwspssolpluvodyiehsrogwmwwdwndsgeeokitabdzcjphfnncfkonthhtlskpmmnaeaeaeaeaezezmzmzmgdaozsvtdkgybgrhjoataabnytkbisolpkidcnfwiyolsapstapmgssfjehkltssaeaeaeaeaezezmzmzmcelrfdwyosdainenfxutbdcxfytsbkgwpmdlbncsmyfylkfpptmypfmulymhchrpaeaeaeaeaezezmzmzmaonblnadaeaeaeaeaecmaebbsfvsswjyhsswjspaehcneymojtdnltbdistluozsrnfxbsaeaeaeaeaecmaebbwflurdurhyzchhoyktfrckwnistabkckidplbbtyglksceaeadadctnblnadaeaeaeaeaecmaebbsfvsswjyhsswjspaehcneymojtdnltbdistluozscpaoaoqzotdwiewnayzsfgzckpotbwbzvylbzosppdwdnyaxryfnkbjpdtwlfmmyjptyfyfddyfeaoclaevendbaiazmrdjtisgwinjsfxjeetosrocerdhglazcpfksgesnbgsefedafemhhnaocxdebkoniypyoxrfksmecyenjppfsbrecsadldfgbntastdeutbyptylbymwttpsjzadcpamaoqzotdwiewnayzsfgzckpotbwbzvylbzosppdwdnyaxryfnkbjpdtwlfmmyjptyfycsbsahinfxghaeaelaadaeaelaaeaeaelaaeaeaeaebeaeaeaeaeadaegmaoaeaeaeadjzfleetolptazmhyrnfmuywtgabaematdnhhsamtdlfrpdamguhljsbtswlnctrpaeaeaeaeaeadaeaeaeadlafpbsaeaeaeaeaecmaebbbnbyprhpjyfnqdfwdmfgctheuofnkbbeknvlsaluaeaeaeaeadadctlafpbsaeaeaeaeaecmaebbbnbyprhpjyfnqdfwdmfgctheuofnkbbeknvlsalucpamaxjshyvldrhnmdzmlodyndcpcflgskiatyrevytswfetknrssrolfxuyiddacpmourcsbsahinfxghaeaelaadaeaelaaeaeaelaaeaeaeaeloaeaeaeaeadaegmaoaeaeaeadrpwzdkvtqdishdmsolvotssrcnnynngomomeylnndldwdagaamdrpeuemwlpflrtaeaeaeaeaeadaeaeaeadtdfpbsaeaeaeaeaecmaebbzcuefnmoaycltpfsdilkctcweemnfhidzofedpmkaeaeaeaeadadcttdfpbsaeaeaeaeaecmaebbzcuefnmoaycltpfsdilkctcweemnfhidzofedpmkcpamaxlrbsksaaiohnrhaogwgmplrenshtbnenregsvygocyfdfwjoeorhhkjownprbbykcsbsahinfxghaeaelaadaeaelaaeaeaelaaeaeaeaelnaeaeaeaecpaoaoadhtuohplrrtesdwsnjsfxhgbwkndtglptgofelblboskpdadanltlcwaxvtcnjncsbsahinfxghaeaelaadaeaelaaeaeaelaaeaeaeaeldaeaeaeaecpaoaohkwtcpwtztchjtqdmkjprovlwnwesoskkbfziefglrtnrnjyrdahbeatlumemndwcsbsahinfxghaeaelaadaeaelaaeaeaelaadaeaeaegmaeaeaeaerdinonfp
```

Once a transaction is fully signed, Keytool can extract the raw Bitcoin transaction, ready to be placed on the network.

```
keytool \
  --psbt "cHNidP8BAMMCAAAAA0DcshmpIS/ZW94f7roto1MhBNhk6Rz/ZaetRRfcedh9AAAAAAD9////wm+mbFIbTYiPlNNhB8P1/vBcwKG/ZryOHMKkL7dSqTYAAAAAAP3///+CmZgUyFrdKNCZUoqfgBhCAFORPdiP/qr0OOt4I5n+CgAAAAAA/f///wJAQg8AAAAAABYAFCBk78yn2RApIqWfuXigrti3WcQRncYQAAAAAAAWABSMyh/Y85oXJKUZQJT+IeAZJk/MeAAAAAAAAQD9PgECAAAABnPYBYJW735g7YMuaFrrK2ZgQj2XELyHukIJmdvOUB4sAAAAAAD+////bmU42jLXSFq3iLHmsyOR/OrF/x0ZQ2lAbecy8CeanlsAAAAAAP7///87WWkGf++EI4Um41R9YUGj00Z0+wvW0fmPYJ+81lWZOAEAAAAA/v///++syYWL4jBkYbhPlOrxJkozfdkL/XJWnhl2nVzVxa2OAAAAAAD+////UAL64CRRErlwBwQM+X5opqpiI0JmpsKs2a1MzGtZh8QAAAAAAP7///8chEjupyVpNkPdCyBE1wpPrS8MGI9EjEGpj7CTgZAXtgAAAAAA/v///wKghgEAAAAAABYAFMzoxnRhxnGxMSMykm4rhwto1dz6vkMPAAAAAAAWABTzi7rfXv1coXc7HvFo2QoeYq4U1E54HAABAR+ghgEAAAAAABYAFMzoxnRhxnGxMSMykm4rhwto1dz6IgICtKMsZPEI+kb9daMTFeF/+8io6poDvTx+cinpPo9y1ERIMEUCIQDkmw5j/7puaE9pcUNrOKe4HLpXgP2weErNEsFFJUWQYAIgKAqlZqukvHiRGjZysMu1GAGJRgzZxyjdEan3EZTRrGwBIgYCtKMsZPEI+kb9daMTFeF/+8io6poDvTx+cinpPo9y1EQYDwVpQ1QAAIABAACAAAAAgAAAAAAQAAAAAAEAUgIAAAABbEc0zoXZ/16+PtvwSQ43BytcwpYvO6gGU11xDcaGH7YAAAAAAAEAAAABgEEPAAAAAAAWABQMEbJbdDyzQi5GH1/cPH4QeuPCiwAAAAABAR+AQQ8AAAAAABYAFAwRslt0PLNCLkYfX9w8fhB648KLIgIDcV7jKmCV/4gwmyIZjcVj1LXh1/M4er/DpkPbYiUikt9IMEUCIQDeFzLuE1DRcQEvwJevA+kf8m8Ay9LJbnoP4qarthudHQIgYyk2ZQEIbvXSFDh37tTG9u2T247SbCfzYNsbLFW4AVYBIgYDcV7jKmCV/4gwmyIZjcVj1LXh1/M4er/DpkPbYiUikt8YDwVpQ1QAAIABAACAAAAAgAAAAACIAAAAAAEAUgIAAAABtvIk4LNoWJem4tfDI5qeVZKR954vLCVJBiqv3pSFR8AAAAAAAAEAAAAB0kEPAAAAAAAWABT93jySCCHYPSeMHxs0jj9i+0UtmAAAAAABAR/SQQ8AAAAAABYAFP3ePJIIIdg9J4wfGzSOP2L7RS2YIgIDhA94BGdguQJPUq61nFoMNrVM4VUaSEJwM7lZcPGyFPVHMEQCIFALHbiTldYsUElBStxS550LVScYBmmsgRrWGWPVfr7EAiAA1kGB/iscnQrbJnKtBhLEDE4MsCfiLjQNNakjB+9MjgEiBgOED3gEZ2C5Ak9SrrWcWgw2tUzhVRpIQnAzuVlw8bIU9RgPBWlDVAAAgAEAAIAAAACAAAAAAIYAAAAAIgICAVrcW4TAOSzNcUNXE3opTqlVRX9/p3UlJZnVGwPgI20YDwVpQ1QAAIABAACAAAAAgAAAAACJAAAAACICAlnwIvD8F26zmHK44/HtycV+QGRGhNq+dLoFEAeLkY4sGA8FaUNUAACAAQAAgAAAAIABAAAAUgAAAAA=" \
  transaction \
  transaction-hex

ur:crypto-tx/hkaoayaoaeaeaeaeadaxfzuoprcfptcldltahpuectwyrddpotguclaatpiewlcezmihospmfechuokktpkiaeaeaeaeaezczmzmzmsajloljzgmcwgtlomymwtehsatsrykzewthhrtoyrsiyrfmncesaoxdlrlgmptenaeaeaeaeaezczmzmzmlfnlmkbbsphtutdetinlgmlenelacsfwaegumefstpmyzepkwketwmkscnnlzebkaeaeaeaeaezczmzmzmaofzfwbsaeaeaeaeaecmaebbcxiewssfostabedtcponnerhksnbpltprlhkssbyntswbeaeaeaeaeaecmaebblksgcttpwfnychdkoncffzmwzeclvtcfdsgwsfksaofddyfeaoclaevendbaiazmrdjtisgwinjsfxjeetosrocerdhglazcpfksgesnbgsefedafemhhnaocxdebkoniypyoxrfksmecyenjppfsbrecsadldfgbntastdeutbyptylbymwttpsjzadclaoqzotdwiewnayzsfgzckpotbwbzvylbzosppdwdnyaxryfnkbjpdtwlfmmyjptyfyaofddyfeaoclaeuecheywybwgdttjsaddlrtmspeaxwlctwzjlaesbtdsojtknbsvoolpyrpcwntcaaocxiadtenihadayjtyktdbbetktwytyswynwemuuymntdjzdiwfhnuycwdwgoroadhfadclaxjshyvldrhnmdzmlodyndcpcflgskiatyrevytswfetknrssrolfxuyiddacpmouraofldyfyaocxgdbdcaromumdtbdwgdgafpgeuogmvdntbdgodicsaminpslycytbcfiatlkbrnssaocxaetbfplyzedncentbkuydsjppmambgssbnglbnpfdivodmeebtecptcnatwsgsmnadclaxlrbsksaaiohnrhaogwgmplrenshtbnenregsvygocyfdfwjoeorhhkjownprbbykaeaeaeaegyzswkhd
0200000000010340dcb219a9212fd95bde1feeba2da3532104d864e91cff65a7ad4517dc79d87d0000000000fdffffffc26fa66c521b4d888f94d36107c3f5fef05cc0a1bf66bc8e1cc2a42fb752a9360000000000fdffffff82999814c85add28d099528a9f8018420053913dd88ffeaaf438eb782399fe0a0000000000fdffffff0240420f00000000001600142064efcca7d9102922a59fb978a0aed8b759c4119dc61000000000001600148cca1fd8f39a1724a5194094fe21e019264fcc7802483045022100e49b0e63ffba6e684f6971436b38a7b81cba5780fdb0784acd12c145254590600220280aa566aba4bc78911a3672b0cbb5180189460cd9c728dd11a9f71194d1ac6c012102b4a32c64f108fa46fd75a31315e17ffbc8a8ea9a03bd3c7e7229e93e8f72d44402483045022100de1732ee1350d171012fc097af03e91ff26f00cbd2c96e7a0fe2a6abb61b9d1d02206329366501086ef5d2143877eed4c6f6ed93db8ed26c27f360db1b2c55b80156012103715ee32a6095ff88309b22198dc563d4b5e1d7f3387abfc3a643db62252292df024730440220500b1db89395d62c5049414adc52e79d0b5527180669ac811ad61963d57ebec4022000d64181fe2b1c9d0adb2672ad0612c40c4e0cb027e22e340d35a92307ef4c8e012103840f78046760b9024f52aeb59c5a0c36b54ce1551a48427033b95970f1b214f500000000
```

Keytool can be invoked repeatedly to add multiple signatures to a PSBT and then extract the final transaction, with the progessively-signed PSBT handed from one invocation to the next using the Unix pipe operator (`|`)

```
PSBT_TO_SIGN=cHNidP8BAMMCAAAAA0DcshmpIS/ZW94f7roto1MhBNhk6Rz/ZaetRRfcedh9AAAAAAD9////wm+mbFIbTYiPlNNhB8P1/vBcwKG/ZryOHMKkL7dSqTYAAAAAAP3///+CmZgUyFrdKNCZUoqfgBhCAFORPdiP/qr0OOt4I5n+CgAAAAAA/f///wJAQg8AAAAAABYAFCBk78yn2RApIqWfuXigrti3WcQRncYQAAAAAAAWABSMyh/Y85oXJKUZQJT+IeAZJk/MeAAAAAAAAQD9PgECAAAABnPYBYJW735g7YMuaFrrK2ZgQj2XELyHukIJmdvOUB4sAAAAAAD+////bmU42jLXSFq3iLHmsyOR/OrF/x0ZQ2lAbecy8CeanlsAAAAAAP7///87WWkGf++EI4Um41R9YUGj00Z0+wvW0fmPYJ+81lWZOAEAAAAA/v///++syYWL4jBkYbhPlOrxJkozfdkL/XJWnhl2nVzVxa2OAAAAAAD+////UAL64CRRErlwBwQM+X5opqpiI0JmpsKs2a1MzGtZh8QAAAAAAP7///8chEjupyVpNkPdCyBE1wpPrS8MGI9EjEGpj7CTgZAXtgAAAAAA/v///wKghgEAAAAAABYAFMzoxnRhxnGxMSMykm4rhwto1dz6vkMPAAAAAAAWABTzi7rfXv1coXc7HvFo2QoeYq4U1E54HAABAR+ghgEAAAAAABYAFMzoxnRhxnGxMSMykm4rhwto1dz6IgYCtKMsZPEI+kb9daMTFeF/+8io6poDvTx+cinpPo9y1EQYDwVpQ1QAAIABAACAAAAAgAAAAAAQAAAAAAEAUgIAAAABbEc0zoXZ/16+PtvwSQ43BytcwpYvO6gGU11xDcaGH7YAAAAAAAEAAAABgEEPAAAAAAAWABQMEbJbdDyzQi5GH1/cPH4QeuPCiwAAAAABAR+AQQ8AAAAAABYAFAwRslt0PLNCLkYfX9w8fhB648KLIgYDcV7jKmCV/4gwmyIZjcVj1LXh1/M4er/DpkPbYiUikt8YDwVpQ1QAAIABAACAAAAAgAAAAACIAAAAAAEAUgIAAAABtvIk4LNoWJem4tfDI5qeVZKR954vLCVJBiqv3pSFR8AAAAAAAAEAAAAB0kEPAAAAAAAWABT93jySCCHYPSeMHxs0jj9i+0UtmAAAAAABAR/SQQ8AAAAAABYAFP3ePJIIIdg9J4wfGzSOP2L7RS2YIgYDhA94BGdguQJPUq61nFoMNrVM4VUaSEJwM7lZcPGyFPUYDwVpQ1QAAIABAACAAAAAgAAAAACGAAAAACICAgFa3FuEwDkszXFDVxN6KU6pVUV/f6d1JSWZ1RsD4CNtGA8FaUNUAACAAQAAgAAAAIAAAAAAiQAAAAAiAgJZ8CLw/Bdus5hyuOPx7cnFfkBkRoTavnS6BRAHi5GOLBgPBWlDVAAAgAEAAIAAAACAAQAAAFIAAAAA
KEY_1=cSyLgU8rSvYNU1j6XR2vo5ebSQqza7PR9iFNtkkYrFMwyB5or5gH
KEY_2=cVok3VJ1fGhdT2yAndKHFWvgTbDsdVFXb8xE9rsAcQASuS4JV9HG
KEY_3=cMaqfcb16JjrCdfYTfT2vRKFpGjdAugAucEAxw3VuhN91XgjKqzF
keytool --network testnet --psbt ${PSBT_TO_SIGN} --address-ec-key-wif ${KEY_1} psbt-signed \
  | keytool --network testnet --psbt '' --address-ec-key-wif ${KEY_2} psbt-signed \
  | keytool --network testnet --psbt '' --address-ec-key-wif ${KEY_3} psbt-signed \
  | keytool --psbt '' transaction-hex
  
0200000000010340dcb219a9212fd95bde1feeba2da3532104d864e91cff65a7ad4517dc79d87d0000000000fdffffffc26fa66c521b4d888f94d36107c3f5fef05cc0a1bf66bc8e1cc2a42fb752a9360000000000fdffffff82999814c85add28d099528a9f8018420053913dd88ffeaaf438eb782399fe0a0000000000fdffffff0240420f00000000001600142064efcca7d9102922a59fb978a0aed8b759c4119dc61000000000001600148cca1fd8f39a1724a5194094fe21e019264fcc7802483045022100e49b0e63ffba6e684f6971436b38a7b81cba5780fdb0784acd12c145254590600220280aa566aba4bc78911a3672b0cbb5180189460cd9c728dd11a9f71194d1ac6c012102b4a32c64f108fa46fd75a31315e17ffbc8a8ea9a03bd3c7e7229e93e8f72d44402483045022100de1732ee1350d171012fc097af03e91ff26f00cbd2c96e7a0fe2a6abb61b9d1d02206329366501086ef5d2143877eed4c6f6ed93db8ed26c27f360db1b2c55b80156012103715ee32a6095ff88309b22198dc563d4b5e1d7f3387abfc3a643db62252292df024730440220500b1db89395d62c5049414adc52e79d0b5527180669ac811ad61963d57ebec4022000d64181fe2b1c9d0adb2672ad0612c40c4e0cb027e22e340d35a92307ef4c8e012103840f78046760b9024f52aeb59c5a0c36b54ce1551a48427033b95970f1b214f500000000
```

## Requesting a Seed

Generate a seed using Seedtool:

```
seedtool

581fbdbf6b3eeababae7e7b51e3aabea
```

Generate the digest of the seed:

```
keytool \
	--seed 581fbdbf6b3eeababae7e7b51e3aabea \
	seed-digest

faf717696382dff46fe473c75936a7df3990cd56e438aaad9953e94fba7f857f
```

Generate a request for the seed with the given digest, printing the request and the request ID:

```
keytool \
	--seed-digest faf717696382dff46fe473c75936a7df3990cd56e438aaad9953e94fba7f857f \
	seed-request \
	seed-request-id

ur:crypto-request/oeadtpdagdrpaswlkntlcegegmrpprgutelrsevdlfaotaadwkhdcxzsylchinialfurwkjlvejksthkenosuresmhsnhfveetpkpmnlguwlgwrdlblplblghtssad
b609e97a-d51c-4a52-b6b2-53d384c1e782
```

Generate a response to the above request, printing the request and the request ID:

```
keytool \
	--seed-request ur:crypto-request/oeadtpdagdrpaswlkntlcegegmrpprgutelrsevdlfaotaadwkhdcxzsylchinialfurwkjlvejksthkenosuresmhsnhfveetpkpmnlguwlgwrdlblplblghtssad \
	--seed 581fbdbf6b3eeababae7e7b51e3aabea \
	seed-response \
	seed-request-id

ur:crypto-response/oeadtpdagdrpaswlkntlcegegmrpprgutelrsevdlfaotaaddwoyadgdhdctryrsjefmwdrdrdvdvdreckftpywdmerfiyrs
b609e97a-d51c-4a52-b6b2-53d384c1e782
```

Extract the hex seed from the above response:

```
keytool \
	--seed-response ur:crypto-response/oeadtpdagdrpaswlkntlcegegmrpprgutelrsevdlfaotaaddwoyadgdhdctryrsjefmwdrdrdvdvdreckftpywdmerfiyrs \
	seed-hex

581fbdbf6b3eeababae7e7b51e3aabea
```

## Requesting a Key Derivation

Generate a master HD key from the above seed:

```
keytool \
	--seed 581fbdbf6b3eeababae7e7b51e3aabea \
	master-key

ur:crypto-hdkey/oxadykaoykaxhdclaeaozmfxfpvdvwmulklywyatdahypmknahkoaoiepkwztkidwevosbsoqdhgltnngoaahdcxdeeyoevetdtpehlslyaaoyhfkideiygytetkvdmechvokptnkoltgojzdllpfwpkrpmtjkhy
```

Get a testnet account derivation path for the master key:

```
keytool \
	--network testnet \
	--master-key ur:crypto-hdkey/oxadykaoykaxhdclaeaozmfxfpvdvwmulklywyatdahypmknahkoaoiepkwztkidwevosbsoqdhgltnngoaahdcxdeeyoevetdtpehlslyaaoyhfkideiygytetkvdmechvokptnkoltgojzdllpfwpkrpmtjkhy \
	account-derivation-path

0fd09078/84h/1h/0h
```

Generate a request for a public key derived from the master key, printing the request and the request ID:

```
keytool \
	--key-request-type public \
	--key-request-derivation-path 0fd09078/84h/1h/0h \
	key-request \
	key-request-id

ur:crypto-request/oeadtpdagdhkckiepsgwqzfyoxnnmhgspfhtoerylraotaadykoeadwkaotaaddyoeadlncsghykadykaeykaocybstimhksfwrlosee
591e64ac-4fb4-44a4-9e90-4cb05aa2bd84
```

Generate a response for the requested derived key.

```
src/keytool \
	--key-request ur:crypto-request/oeadtpdagdhkckiepsgwqzfyoxnnmhgspfhtoerylraotaadykoeadwkaotaaddyoeadlncsghykadykaeykaocybstimhksfwrlosee \
	--source-key ur:crypto-hdkey/oxadykaoykaxhdclaeaozmfxfpvdvwmulklywyatdahypmknahkoaoiepkwztkidwevosbsoqdhgltnngoaahdcxdeeyoevetdtpehlslyaaoyhfkideiygytetkvdmechvokptnkoltgojzdllpfwpkrpmtjkhy \
	key-response

ur:crypto-response/oeadtpdagdhkckiepsgwqzfyoxnnmhgspfhtoerylraotaaddloxaxhdclaondcfnsctgmqzgmcnpyurahykcfjklkcnjeolcfkimhattbkgglmtjlpddrwpzcktaahdcxrftpgmaoayaefgzmplesjskicmlamktnhgssmucnrytocmcftacxwlcedelneelyamoeadlncsghykadykaeykaocybstimhksaycywtwpeoeylgcnfysp
```

From the response, extract the key in UR and Base58 formats, and the request ID:

```
keytool \
	--key-response ur:crypto-response/oeadtpdagdhkckiepsgwqzfyoxnnmhgspfhtoerylraotaaddloxaxhdclaondcfnsctgmqzgmcnpyurahykcfjklkcnjeolcfkimhattbkgglmtjlpddrwpzcktaahdcxrftpgmaoayaefgzmplesjskicmlamktnhgssmucnrytocmcftacxwlcedelneelyamoeadlncsghykadykaeykaocybstimhksaycywtwpeoeylgcnfysp \
	derived-key \
	derived-key-base58 \
	key-request-id

ur:crypto-hdkey/oxaxhdclaondcfnsctgmqzgmcnpyurahykcfjklkcnjeolcfkimhattbkgglmtjlpddrwpzcktaahdcxrftpgmaoayaefgzmplesjskicmlamktnhgssmucnrytocmcftacxwlcedelneelyamoeadlncsghykadykaeykaocybstimhksaycywtwpeoeyjohkterk
xpub6DRUbxFn5yqrAkKgnsUNjPcbw5NtFjQnRNK4ytXwCxMKv3rks66xPKWK5YBUy9sDddCpta7FexygbmKUp7shSKeMRk1cWeDQSXvGyvX9s1i
591e64ac-4fb4-44a4-9e90-4cb05aa2bd84
```

## Derivable and Non-Derivable Keys

Hierarchical Deterministic (HD) keys contain a field called the "chain code" that enables a key to be used to derive further keys. If the chain code is omitted, the key can still be used to produce Elliptic Curve (EC) keys used to verify transactions (or sign them if it is private key), but no further HD keys can be derived from it. The `ur:crypto-hdkey` format supports such "non-derivable" keys, and Keytool enables generating and using such keys via the `is-derivable` node. The `is-derivable` node contains a boolean value that, if set to `false`, causes keys derived from the `address-key` or `derived-key` nodes to be non-derivable. In addition, setting `is-derivable` to `false` causes generated `ur:crypto-requests` for keys to become requests for non-derivable keys, and causes generated `ur:crypto-response`s to produce non-derivable keys.

The Base58 format for HD keys only supports derivable keys, so Keytool will produce an error if asked to produce a Base58 key from a non-derivable HD key.

Generate derivable and non-derivable versions of a key:

```
keytool \
	--seed 581fbdbf6b3eeababae7e7b51e3aabea \
	address-key

ur:crypto-hdkey/onaoykaxhdclaebkmdcfjtdmaacybbesmskbolvdvalbdkgachhtlefpcmotltoezsynjlehidmokeaahdcxjsvdzechamzcosfhsrksyasnurgturdposknjsteaomulkfswnrfaenbclhtzcwnamoeadlecsghykaeykaeykaewkaewkaocybstimhksaycytssgtkwkksskatmk

keytool \
	--seed 581fbdbf6b3eeababae7e7b51e3aabea \
	--is-derivable false \
	address-key

ur:crypto-hdkey/oxaoykaxhdclaebkmdcfjtdmaacybbesmskbolvdvalbdkgachhtlefpcmotltoezsynjlehidmokeamoeadlecsghykaeykaeykaewkaewkaocybstimhksaycytssgtkwkvatnuylf
```

Have Keytool report whether the keys generated above are or are not derivable:

```
keytool \
	--source-key ur:crypto-hdkey/onaoykaxhdclaebkmdcfjtdmaacybbesmskbolvdvalbdkgachhtlefpcmotltoezsynjlehidmokeaahdcxjsvdzechamzcosfhsrksyasnurgturdposknjsteaomulkfswnrfaenbclhtzcwnamoeadlecsghykaeykaeykaewkaewkaocybstimhksaycytssgtkwkksskatmk \
	is-derivable

true

keytool \
	--source-key ur:crypto-hdkey/oxaoykaxhdclaebkmdcfjtdmaacybbesmskbolvdvalbdkgachhtlefpcmotltoezsynjlehidmokeamoeadlecsghykaeykaeykaewkaewkaocybstimhksaycytssgtkwkvatnuylf \
	is-derivable

false
```

Attempt to derive a key from the derivable and non-derivable keys generated above:

```
keytool \
	--source-key ur:crypto-hdkey/onaoykaxhdclaebkmdcfjtdmaacybbesmskbolvdvalbdkgachhtlefpcmotltoezsynjlehidmokeaahdcxjsvdzechamzcosfhsrksyasnurgturdposknjsteaomulkfswnrfaenbclhtzcwnamoeadlecsghykaeykaeykaewkaewkaocybstimhksaycytssgtkwkksskatmk \
	--key-request-derivation-path 0 \
	derived-key

ur:crypto-hdkey/onaoykaxhdclaebzgybzdkvdctayyknntdsrgdpfurgmpyjthlkpzeynzthdislaetutdicsjtneuoaahdcxresaftkttytazthfticfjloychchmyjpotmybzrocwaofnotenvomtsnmovwqztdamoeadlkcsghykaeykaeykaewkaewkaewkaocybstimhksaycytacyrypfoswszcrh

keytool \
	--source-key ur:crypto-hdkey/oxaoykaxhdclaebkmdcfjtdmaacybbesmskbolvdvalbdkgachhtlefpcmotltoezsynjlehidmokeamoeadlecsghykaeykaeykaewkaewkaocybstimhksaycytssgtkwkvatnuylf \
	--key-request-derivation-path 0 \
	derived-key

keytool: Cannot derive from a non-derivable key.
```

Produce a request and the corresponding response for the non-derivable key above, and also print the key itself.

```
keytool \
	--seed 581fbdbf6b3eeababae7e7b51e3aabea \
	--is-derivable false \
	key-request \
	key-response \
	derived-key

ur:crypto-request/oeadtpdagdpratsratkisffymontntdmbsbnlramsnaotaadykotadykaotaaddyoeadlecsghykaeykaeykaewkaewkaocybstimhksaawkwzsogalt
ur:crypto-response/oeadtpdagdpratsratkisffymontntdmbsbnlramsnaotaaddloxaoykaxhdclaebkmdcfjtdmaacybbesmskbolvdvalbdkgachhtlefpcmotltoezsynjlehidmokeamoeadlecsghykaeykaeykaewkaewkaocybstimhksaycytssgtkwkjpqddidk
ur:crypto-hdkey/oxaoykaxhdclaebkmdcfjtdmaacybbesmskbolvdvalbdkgachhtlefpcmotltoezsynjlehidmokeamoeadlecsghykaeykaeykaewkaewkaocybstimhksaycytssgtkwkvatnuylf
```

Transform the derivable key above into a non-derivable key:

```
keytool \
	--source-key ur:crypto-hdkey/onaoykaxhdclaebkmdcfjtdmaacybbesmskbolvdvalbdkgachhtlefpcmotltoezsynjlehidmokeaahdcxjsvdzechamzcosfhsrksyasnurgturdposknjsteaomulkfswnrfaenbclhtzcwnamoeadlecsghykaeykaeykaewkaewkaocybstimhksaycytssgtkwkksskatmk \
	--key-request-derivation-path 'm' \
	--is-derivable false \
	derived-key

ur:crypto-hdkey/oxaoykaxhdclaebkmdcfjtdmaacybbesmskbolvdvalbdkgachhtlefpcmotltoezsynjlehidmokeamoeadlecsghykaeykaeykaewkaewkaocybstimhksaycytssgtkwkvatnuylf
```

## Version History

### 0.6.0, 4/1/2021

* Support for non-derivable keys via the `is-derivable` node.

### 0.5.0, 3/29/2021

* UR is now the primary format for types that support it, e.g. `master-key`. Such nodes can still take other forms of input (for example HD Key nodes can take UR or Base58) but output in Base58 is now done via separate nodes (e.g., `master-key-base58`).
* `network` is no longer derived from `asset`; they are separate. The two supported assets are currently Bitcoin `btc` and Ethereum `eth`. To use Bitcoin Testnet, use asset `btc` (the default) and network `testnet`.
* Support for UR:CRYPTO-REQUEST and UR:CRYPTO-RESPONSE types for requesting and returning seeds and derived keys.

### 0.4.0, 11/12/2020

* Added ability to input PSBTs in hex and ur:crypto-psbt formats as well as Base-64, and output PSBTs in all three formats as well.
* Added ability to input transactions in ur:crypto-tx format as well as hex, and output transactions in both formats as well.

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
