#!/bin/bash

# Echo any provided arguments.
# [ $# -gt 0 ] && echo "#:$# 1:$1 2:$2 3:$3"

KEYTOOL=${1:-keytool}

testDefaults()
{
  # asset
  assertEquals $'btc' "$(${KEYTOOL} asset)"

  # account-index
  assertEquals $'0' "$(${KEYTOOL} account-index)"

  # address-index
  assertEquals $'0' "$(${KEYTOOL} address-index)"

  # chain-type
  assertEquals $'external' "$(${KEYTOOL} chain-type)"

  # output-type
  assertEquals $'wpkh' "$(${KEYTOOL} output-type)"

  # purpose
  assertEquals $'84' "$(${KEYTOOL} purpose)"
}

testMultilineOutput()
{
  assertEquals $'btc\n84' "$(${KEYTOOL} asset purpose)"
}

testNetwork()
{
  # network <- [asset]
  assertEquals $'mainnet' "$(${KEYTOOL} network)"

  # network <- [asset]
  assertEquals $'testnet' "$(${KEYTOOL} --asset btct network)"
}

testCoinType()
{
  # coin-type <- [asset]
  assertEquals $'0' "$(${KEYTOOL} coin-type)"
}

testChainTypeInt()
{
  # chain-type-int <- [chain-type]
  assertEquals $'0' "$(${KEYTOOL} chain-type-int)"
  assertEquals $'1' "$(${KEYTOOL} --chain-type change chain-type-int)"
}

testSeed()
{
  # seed <- hex
  assertEquals $'1b1454fab426f2729ddcec1b2a6fb539aa2ff0a36d078902994e2fde561d6550' \
    "$(${KEYTOOL} --seed 1b1454fab426f2729ddcec1b2a6fb539aa2ff0a36d078902994e2fde561d6550 seed)"
}

testSeedUROutput()
{
  # seed <- hex
  assertEquals $'ur:crypto-seed/oyadhdcxcwbbghzsqzdswzjpntuowpcwdrjlreespkdlwtotjnatldaonlgldluehfcaihgdrddevese' \
    "$(${KEYTOOL} --seed 1b1454fab426f2729ddcec1b2a6fb539aa2ff0a36d078902994e2fde561d6550 seed-ur)"
}

testSeedURInput()
{
  # seed <- hex
  assertEquals $'1b1454fab426f2729ddcec1b2a6fb539aa2ff0a36d078902994e2fde561d6550' \
    "$(${KEYTOOL} --seed ur:crypto-seed/oyadhdcxcwbbghzsqzdswzjpntuowpcwdrjlreespkdlwtotjnatldaonlgldluehfcaihgdrddevese seed)"
}

testMasterKey()
{
  # master-key <- [network, seed]
  assertEquals $'xprv9s21ZrQH143K28kcT3e8kegkeMexRDzozueBMsHVtctk8gYxLUiBVqzZpK8KwcTjz5xnzZq84ymFKTmbXqwdexKJRkSuejVcCAM8P7sc39b' \
    "$(${KEYTOOL} --seed 1b1454fab426f2729ddcec1b2a6fb539aa2ff0a36d078902994e2fde561d6550 master-key)"
}

testMasterKeyFingerprint()
{
  # master-key-fingerprint <- [master-key]
  assertEquals $'01577231' \
    "$(${KEYTOOL} --seed 1b1454fab426f2729ddcec1b2a6fb539aa2ff0a36d078902994e2fde561d6550 master-key-fingerprint)"
}

testAccountDerivationPath()
{
  # account-derivation-path <- [master-key-fingerprint, purpose, coin-type, account-index]
  assertEquals $'m/84h/0h/0h' "$(${KEYTOOL} account-derivation-path)"
  assertEquals $'9abcdef0/84h/0h/0h' "$(${KEYTOOL} --master-key-fingerprint 9abcdef0 account-derivation-path)"
}

testAccountKey()
{
  # account-key <- [master-key, account-derivation-path]
  assertEquals $'xprv9yasgNa354kXD4zJNZbS9tAsDXnENsGdJbrRkrwvtyb348XhccD42B8ekimwxyj6dNy6jX4K7NpUoNNaxTrCjrWMgFGTcaqZbwpevvwfBeR' \
    "$(${KEYTOOL} --master-key xprv9s21ZrQH143K28kcT3e8kegkeMexRDzozueBMsHVtctk8gYxLUiBVqzZpK8KwcTjz5xnzZq84ymFKTmbXqwdexKJRkSuejVcCAM8P7sc39b account-key)"
}

testPartialAddressDerivationPath()
{
    # address-derivation-path <- [chain-type, address-index]
    assertEquals $'0/0' "$(${KEYTOOL} address-derivation-path)"
    assertEquals $'1/*' "$(${KEYTOOL} --chain-type change --address-index '*' address-derivation-path)"
}

testFullAddressDerivationPath()
{
  # full-address-derivation-path <- [account-derivation-path, address-derivation-path]
  assertEquals $'m/84h/0h/0h/0/0' "$(${KEYTOOL} full-address-derivation-path)"
}

testAccountPubKey()
{
  # account-pub-key <- [account-key]
  assertEquals $'xpub6DDytLvRh5N8QR3H4nzY4q3TCStCsnmLBQtjNE3rNFcEyhvvNF3yN6WTLyK599xkUdEKycY5cxcujv9u9YeiENgYBewfCUUHXdQmzj3fjqo' \
    "$(${KEYTOOL} --account-key xprv9zEdUqPXrhoqBvxoxmTXhh6ieR3iUL3UpBy8ZqeEov5G6ubmphjipJByVhoxBnRtF5LhgrCSyQLEDKLqKoBVJHzDHXZBkhXmfbDje6Bm7AQ account-pub-key)"
}

testAddressKey()
{
  # address-key <- [master-key, full-address-derivation-path]
  assertEquals $'xprvA3wj42STH7XPmjS3eSAQiZKowik7pky9r3hck3jNVWA2tHBXyn92mtC3M6CJDSbG92av7CJF8Zr64gsb4s9LQkQvGi5E1QvAFgvuVk3bEqE' \
    "$(${KEYTOOL} --master-key xprv9s21ZrQH143K28kcT3e8kegkeMexRDzozueBMsHVtctk8gYxLUiBVqzZpK8KwcTjz5xnzZq84ymFKTmbXqwdexKJRkSuejVcCAM8P7sc39b address-key)"

  # address-key <- [account-key, address-derivation-path]
  assertEquals $'xprvA2p2Wu4w5XUZ6L2iHjvuxCkm38bUcwNCKwSB9XXG5oMjArwtkm2TeA3amg5p73QZVH6owECFo4T9nGnCEuwgUMWLbivB24yyjpsWBWPvYgf' \
    "$(${KEYTOOL} --account-key xprv9zEdUqPXrhoqBvxoxmTXhh6ieR3iUL3UpBy8ZqeEov5G6ubmphjipJByVhoxBnRtF5LhgrCSyQLEDKLqKoBVJHzDHXZBkhXmfbDje6Bm7AQ address-key)"
}

testAddressPubKey()
{
  # address-pub-key <- [address-key]
  assertEquals $'xpub6FoNvQbpuu2rJp7BPmTvKLhVbARy2Q63hAMmwuvse8ti3fH3JJLiBxN4cyXjeypWSen5ydkxfxJCLkPydGqQrSdhAwxe9ExFzbakNZ46AK6' \
    "$(${KEYTOOL} --address-key xprvA2p2Wu4w5XUZ6L2iHjvuxCkm38bUcwNCKwSB9XXG5oMjArwtkm2TeA3amg5p73QZVH6owECFo4T9nGnCEuwgUMWLbivB24yyjpsWBWPvYgf address-pub-key)"

  # address-pub-key <- [account-pub-key, address-derivation-path]
  assertEquals $'xpub6FoNvQbpuu2rJp7BPmTvKLhVbARy2Q63hAMmwuvse8ti3fH3JJLiBxN4cyXjeypWSen5ydkxfxJCLkPydGqQrSdhAwxe9ExFzbakNZ46AK6' \
    "$(${KEYTOOL} --account-pub-key xpub6DDytLvRh5N8QR3H4nzY4q3TCStCsnmLBQtjNE3rNFcEyhvvNF3yN6WTLyK599xkUdEKycY5cxcujv9u9YeiENgYBewfCUUHXdQmzj3fjqo address-pub-key)"
}

testAddressECKey()
{
  # address-ec-key <- [address-key]
  assertEquals $'063e2110fc78e7944175d6714a77bc33f5efac9ba25a575980918dca3a738323' \
    "$(${KEYTOOL} --address-key xprvA2p2Wu4w5XUZ6L2iHjvuxCkm38bUcwNCKwSB9XXG5oMjArwtkm2TeA3amg5p73QZVH6owECFo4T9nGnCEuwgUMWLbivB24yyjpsWBWPvYgf address-ec-key)"

  # address-ec-key <- [address-ec-key-wif]
  assertEquals $'063e2110fc78e7944175d6714a77bc33f5efac9ba25a575980918dca3a738323' \
    "$(${KEYTOOL} --address-ec-key-wif KwRr1isWZUTRWhrWL2gTyoyuABXSnADtLbryFozgxKHbPDBoWpoU address-ec-key)"
}

testAddressPubECKey()
{
  # address-pub-ec-key <- [address-ec-key]
  assertEquals $'03447c85461b3383b7077d678a79dde4ba09c74da2036811b5e6165f1ae6254dd6' \
    "$(${KEYTOOL} --address-ec-key 063e2110fc78e7944175d6714a77bc33f5efac9ba25a575980918dca3a738323 address-pub-ec-key)"

  # address-pub-ec-key <- [address-pub-key]
  assertEquals $'03447c85461b3383b7077d678a79dde4ba09c74da2036811b5e6165f1ae6254dd6' \
    "$(${KEYTOOL} --address-pub-key xpub6FoNvQbpuu2rJp7BPmTvKLhVbARy2Q63hAMmwuvse8ti3fH3JJLiBxN4cyXjeypWSen5ydkxfxJCLkPydGqQrSdhAwxe9ExFzbakNZ46AK6 address-pub-ec-key)"
}

testAddressECKeyWIF()
{
  # address-ec-key-wif <- [address-ec-key, network]
  assertEquals $'KwRr1isWZUTRWhrWL2gTyoyuABXSnADtLbryFozgxKHbPDBoWpoU' \
    "$(${KEYTOOL} --address-ec-key 063e2110fc78e7944175d6714a77bc33f5efac9ba25a575980918dca3a738323 address-ec-key-wif)"
}

testAddressPKH()
{
  # address-pkh <- [address-pub-ec-key, asset]
  assertEquals $'1AwGNpYoJBxTQb2cFBHPS78L2AqsT9rAjm' \
    "$(${KEYTOOL} --address-pub-ec-key 03447c85461b3383b7077d678a79dde4ba09c74da2036811b5e6165f1ae6254dd6 address-pkh)"
}

testAddressSH()
{
  # address-sh <- [address-pub-ec-key, asset]
  assertEquals $'3BdHJN3Er6GqVkj3NGwyrjVGAh8aybp2ou' \
    "$(${KEYTOOL} --address-pub-ec-key 03447c85461b3383b7077d678a79dde4ba09c74da2036811b5e6165f1ae6254dd6 address-sh)"
}

testOutputDescriptor()
{
  # output-descriptor <- [output-descriptor-type, account-derivation-path, address-derivation-path, account-pub-key]
  assertEquals $'sh(wpkh([01577231/49h/0h/0h]xpub6DMNt7GPfYj4XkPio3LdjTsofQWTfYAM1ryRLpwU5XQ17xpiSuKicFBL5uD42RRXeFAFvHepiFw5xpRqGhCqj2QgmQ5ZTJCXXhY2VUU6xvw/0/*))' \
    "$(${KEYTOOL} --seed 1b1454fab426f2729ddcec1b2a6fb539aa2ff0a36d078902994e2fde561d6550 --output-type sh-wpkh --address-index '*' output-descriptor)"
}

testPSBT()
(
    # PSBT test vectors from https://github.com/bitcoin/bips/blob/master/bip-0174.mediawiki
    combined_psbt="cHNidP8BAJoCAAAAAljoeiG1ba8MI76OcHBFbDNvfLqlyHV5JPVFiHuyq911AAAAAAD/////g40EJ9DsZQpoqka7CwmK6kQiwHGyyng1Kgd5WdB86h0BAAAAAP////8CcKrwCAAAAAAWABTYXCtx0AYLCcmIauuBXlCZHdoSTQDh9QUAAAAAFgAUAK6pouXw+HaliN9VRuh0LR2HAI8AAAAAAAEAuwIAAAABqtc5MQGL0l+ErkALaISL4J23BurCrBgpi6vucatlb4sAAAAASEcwRAIgWPb8fGoz4bMVSNSByCbAFb0wE1qtQs1neQ2rZtKtJDsCIEoc7SYExnNbY5PltBaR3XiwDwxZQvufdRhW+qk4FX26Af7///8CgPD6AgAAAAAXqRQPuUY0IWlrgsgzryQceMF9295JNIfQ8gonAQAAABepFCnKdPigj4GZlCgYXJe12FLkBj9hh2UAAAAiAgKVg785rgpgl0etGZrd1jT6YQhVnWxc05tMIYPxq5bgf0cwRAIgdAGK1BgAl7hzMjwAFXILNoTMgSOJEEjn282bVa1nnJkCIHPTabdA4+tT3O+jOCPIBwUUylWn3ZVE8VfBZ5EyYRGMASICAtq2H/SaFNtqfQKwzR+7ePxLGDErW05U2uTbovv+9TbXSDBFAiEA9hA4swjcHahlo0hSdG8BV3KTQgjG0kRUOTzZm98iF3cCIAVuZ1pnWm0KArhbFOXikHTYolqbV2C+ooFvZhkQoAbqAQEDBAEAAAABBEdSIQKVg785rgpgl0etGZrd1jT6YQhVnWxc05tMIYPxq5bgfyEC2rYf9JoU22p9ArDNH7t4/EsYMStbTlTa5Nui+/71NtdSriIGApWDvzmuCmCXR60Zmt3WNPphCFWdbFzTm0whg/GrluB/ENkMak8AAACAAAAAgAAAAIAiBgLath/0mhTban0CsM0fu3j8SxgxK1tOVNrk26L7/vU21xDZDGpPAAAAgAAAAIABAACAAAEBIADC6wsAAAAAF6kUt/X69A49QKWkWbHbNTXyty+pIeiHIgIDCJ3BDHrG21T5EymvYXMz2ziM6tDCMfcjN50bmQMLAtxHMEQCIGLrelVhB6fHP0WsSrWh3d9vcHX7EnWWmn84Pv/3hLyyAiAMBdu3Rw2/LwhVfdNWxzJcHtMJE+mWzThAlF2xIijaXwEiAgI63ZBPPW3PWd25BrDe4jUpt/+57VDl6GFRkmhgIh8Oc0cwRAIgZfRbpZmLWaJ//hp77QFq8fH5DVSzqo90UKpfVqJRA70CIH9yRwOtHtuWaAsoS1bU/8uI9/t1nqu+CKow8puFE4PSAQEDBAEAAAABBCIAIIwjUxc3Q7WV37Sge3K6jkLjeX2nTof+fZ10l+OyAokDAQVHUiEDCJ3BDHrG21T5EymvYXMz2ziM6tDCMfcjN50bmQMLAtwhAjrdkE89bc9Z3bkGsN7iNSm3/7ntUOXoYVGSaGAiHw5zUq4iBgI63ZBPPW3PWd25BrDe4jUpt/+57VDl6GFRkmhgIh8OcxDZDGpPAAAAgAAAAIADAACAIgYDCJ3BDHrG21T5EymvYXMz2ziM6tDCMfcjN50bmQMLAtwQ2QxqTwAAAIAAAACAAgAAgAAiAgOppMN/WZbTqiXbrGtXCvBlA5RJKUJGCzVHU+2e7KWHcRDZDGpPAAAAgAAAAIAEAACAACICAn9jmXV9Lv9VoTatAsaEsYOLZVbl8bazQoKpS2tQBRCWENkMak8AAACAAAAAgAUAAIAA"
    finalized_psbt="cHNidP8BAJoCAAAAAljoeiG1ba8MI76OcHBFbDNvfLqlyHV5JPVFiHuyq911AAAAAAD/////g40EJ9DsZQpoqka7CwmK6kQiwHGyyng1Kgd5WdB86h0BAAAAAP////8CcKrwCAAAAAAWABTYXCtx0AYLCcmIauuBXlCZHdoSTQDh9QUAAAAAFgAUAK6pouXw+HaliN9VRuh0LR2HAI8AAAAAAAEAuwIAAAABqtc5MQGL0l+ErkALaISL4J23BurCrBgpi6vucatlb4sAAAAASEcwRAIgWPb8fGoz4bMVSNSByCbAFb0wE1qtQs1neQ2rZtKtJDsCIEoc7SYExnNbY5PltBaR3XiwDwxZQvufdRhW+qk4FX26Af7///8CgPD6AgAAAAAXqRQPuUY0IWlrgsgzryQceMF9295JNIfQ8gonAQAAABepFCnKdPigj4GZlCgYXJe12FLkBj9hh2UAAAABB9oARzBEAiB0AYrUGACXuHMyPAAVcgs2hMyBI4kQSOfbzZtVrWecmQIgc9Npt0Dj61Pc76M4I8gHBRTKVafdlUTxV8FnkTJhEYwBSDBFAiEA9hA4swjcHahlo0hSdG8BV3KTQgjG0kRUOTzZm98iF3cCIAVuZ1pnWm0KArhbFOXikHTYolqbV2C+ooFvZhkQoAbqAUdSIQKVg785rgpgl0etGZrd1jT6YQhVnWxc05tMIYPxq5bgfyEC2rYf9JoU22p9ArDNH7t4/EsYMStbTlTa5Nui+/71NtdSrgABASAAwusLAAAAABepFLf1+vQOPUClpFmx2zU18rcvqSHohwEHIyIAIIwjUxc3Q7WV37Sge3K6jkLjeX2nTof+fZ10l+OyAokDAQjaBABHMEQCIGLrelVhB6fHP0WsSrWh3d9vcHX7EnWWmn84Pv/3hLyyAiAMBdu3Rw2/LwhVfdNWxzJcHtMJE+mWzThAlF2xIijaXwFHMEQCIGX0W6WZi1mif/4ae+0BavHx+Q1Us6qPdFCqX1aiUQO9AiB/ckcDrR7blmgLKEtW1P/LiPf7dZ6rvgiqMPKbhROD0gFHUiEDCJ3BDHrG21T5EymvYXMz2ziM6tDCMfcjN50bmQMLAtwhAjrdkE89bc9Z3bkGsN7iNSm3/7ntUOXoYVGSaGAiHw5zUq4AIgIDqaTDf1mW06ol26xrVwrwZQOUSSlCRgs1R1Ptnuylh3EQ2QxqTwAAAIAAAACABAAAgAAiAgJ/Y5l1fS7/VaE2rQLGhLGDi2VW5fG2s0KCqUtrUAUQlhDZDGpPAAAAgAAAAIAFAACAAA=="
    transaction="0200000000010258e87a21b56daf0c23be8e7070456c336f7cbaa5c8757924f545887bb2abdd7500000000da00473044022074018ad4180097b873323c0015720b3684cc8123891048e7dbcd9b55ad679c99022073d369b740e3eb53dcefa33823c8070514ca55a7dd9544f157c167913261118c01483045022100f61038b308dc1da865a34852746f015772934208c6d24454393cd99bdf2217770220056e675a675a6d0a02b85b14e5e29074d8a25a9b5760bea2816f661910a006ea01475221029583bf39ae0a609747ad199addd634fa6108559d6c5cd39b4c2183f1ab96e07f2102dab61ff49a14db6a7d02b0cd1fbb78fc4b18312b5b4e54dae4dba2fbfef536d752aeffffffff838d0427d0ec650a68aa46bb0b098aea4422c071b2ca78352a077959d07cea1d01000000232200208c2353173743b595dfb4a07b72ba8e42e3797da74e87fe7d9d7497e3b2028903ffffffff0270aaf00800000000160014d85c2b71d0060b09c9886aeb815e50991dda124d00e1f5050000000016001400aea9a2e5f0f876a588df5546e8742d1d87008f000400473044022062eb7a556107a7c73f45ac4ab5a1dddf6f7075fb1275969a7f383efff784bcb202200c05dbb7470dbf2f08557dd356c7325c1ed30913e996cd3840945db12228da5f01473044022065f45ba5998b59a27ffe1a7bed016af1f1f90d54b3aa8f7450aa5f56a25103bd02207f724703ad1edb96680b284b56d4ffcb88f7fb759eabbe08aa30f29b851383d20147522103089dc10c7ac6db54f91329af617333db388cead0c231f723379d1b99030b02dc21023add904f3d6dcf59ddb906b0dee23529b7ffb9ed50e5e86151926860221f0e7352ae00000000"

    assertEquals $combined_psbt \
      "$(${KEYTOOL} --psbt ${combined_psbt} psbt)"

    assertEquals $finalized_psbt \
      "$(${KEYTOOL} --psbt ${combined_psbt} psbt-finalized)"

    # Assigning a non-finalized PSBT to psbt-finalized should fail.
    assertEquals $'' \
      "$(${KEYTOOL} 2>/dev/null --psbt-finalized ${combined_psbt} psbt-finalized)"

    assertEquals $transaction \
      "$(${KEYTOOL} --transaction ${transaction} transaction)"

    assertEquals $transaction \
      "$(${KEYTOOL} --psbt ${combined_psbt} transaction)"
)

testPSBT2()
(
  # PSBT test vectors from https://github.com/bitcoin/bips/blob/master/bip-0174.mediawiki
  master_key=tprv8ZgxMBicQKsPd9TeAdPADNnSyH9SSUUbTVeFszDE23Ki6TBB5nCefAdHkK8Fm3qMQR6sHwA56zqRmKmxnHk37JkiFzvncDqoKmPWubu7hDF
  key1=cP53pDbR5WtAD8dYAW9hhTjuvvTVaEiQBdrz9XPrgLBeRFiyCbQr
  key2=cR6SXDoyfQrcp4piaiHE97Rsgta9mNhGTen9XeonVgwsh4iSgw6d

  assertEquals $key1 \
    "$(${KEYTOOL} --asset btct --master-key ${master_key} --full-address-derivation-path m/0h/0h/0h address-ec-key-wif)"

  assertEquals $key2 \
    "$(${KEYTOOL} --asset btct --master-key ${master_key} --full-address-derivation-path m/0h/0h/2h address-ec-key-wif)"
)

testPSBTSigning()
(
  psbt_to_sign=cHNidP8BAMMCAAAAA0DcshmpIS/ZW94f7roto1MhBNhk6Rz/ZaetRRfcedh9AAAAAAD9////wm+mbFIbTYiPlNNhB8P1/vBcwKG/ZryOHMKkL7dSqTYAAAAAAP3///+CmZgUyFrdKNCZUoqfgBhCAFORPdiP/qr0OOt4I5n+CgAAAAAA/f///wJAQg8AAAAAABYAFCBk78yn2RApIqWfuXigrti3WcQRncYQAAAAAAAWABSMyh/Y85oXJKUZQJT+IeAZJk/MeAAAAAAAAQD9PgECAAAABnPYBYJW735g7YMuaFrrK2ZgQj2XELyHukIJmdvOUB4sAAAAAAD+////bmU42jLXSFq3iLHmsyOR/OrF/x0ZQ2lAbecy8CeanlsAAAAAAP7///87WWkGf++EI4Um41R9YUGj00Z0+wvW0fmPYJ+81lWZOAEAAAAA/v///++syYWL4jBkYbhPlOrxJkozfdkL/XJWnhl2nVzVxa2OAAAAAAD+////UAL64CRRErlwBwQM+X5opqpiI0JmpsKs2a1MzGtZh8QAAAAAAP7///8chEjupyVpNkPdCyBE1wpPrS8MGI9EjEGpj7CTgZAXtgAAAAAA/v///wKghgEAAAAAABYAFMzoxnRhxnGxMSMykm4rhwto1dz6vkMPAAAAAAAWABTzi7rfXv1coXc7HvFo2QoeYq4U1E54HAABAR+ghgEAAAAAABYAFMzoxnRhxnGxMSMykm4rhwto1dz6IgYCtKMsZPEI+kb9daMTFeF/+8io6poDvTx+cinpPo9y1EQYDwVpQ1QAAIABAACAAAAAgAAAAAAQAAAAAAEAUgIAAAABbEc0zoXZ/16+PtvwSQ43BytcwpYvO6gGU11xDcaGH7YAAAAAAAEAAAABgEEPAAAAAAAWABQMEbJbdDyzQi5GH1/cPH4QeuPCiwAAAAABAR+AQQ8AAAAAABYAFAwRslt0PLNCLkYfX9w8fhB648KLIgYDcV7jKmCV/4gwmyIZjcVj1LXh1/M4er/DpkPbYiUikt8YDwVpQ1QAAIABAACAAAAAgAAAAACIAAAAAAEAUgIAAAABtvIk4LNoWJem4tfDI5qeVZKR954vLCVJBiqv3pSFR8AAAAAAAAEAAAAB0kEPAAAAAAAWABT93jySCCHYPSeMHxs0jj9i+0UtmAAAAAABAR/SQQ8AAAAAABYAFP3ePJIIIdg9J4wfGzSOP2L7RS2YIgYDhA94BGdguQJPUq61nFoMNrVM4VUaSEJwM7lZcPGyFPUYDwVpQ1QAAIABAACAAAAAgAAAAACGAAAAACICAgFa3FuEwDkszXFDVxN6KU6pVUV/f6d1JSWZ1RsD4CNtGA8FaUNUAACAAQAAgAAAAIAAAAAAiQAAAAAiAgJZ8CLw/Bdus5hyuOPx7cnFfkBkRoTavnS6BRAHi5GOLBgPBWlDVAAAgAEAAIAAAACAAQAAAFIAAAAA
  key1=cSyLgU8rSvYNU1j6XR2vo5ebSQqza7PR9iFNtkkYrFMwyB5or5gH
  key2=cVok3VJ1fGhdT2yAndKHFWvgTbDsdVFXb8xE9rsAcQASuS4JV9HG
  key3=cMaqfcb16JjrCdfYTfT2vRKFpGjdAugAucEAxw3VuhN91XgjKqzF
  final_transaction=0200000000010340dcb219a9212fd95bde1feeba2da3532104d864e91cff65a7ad4517dc79d87d0000000000fdffffffc26fa66c521b4d888f94d36107c3f5fef05cc0a1bf66bc8e1cc2a42fb752a9360000000000fdffffff82999814c85add28d099528a9f8018420053913dd88ffeaaf438eb782399fe0a0000000000fdffffff0240420f00000000001600142064efcca7d9102922a59fb978a0aed8b759c4119dc61000000000001600148cca1fd8f39a1724a5194094fe21e019264fcc7802483045022100e49b0e63ffba6e684f6971436b38a7b81cba5780fdb0784acd12c145254590600220280aa566aba4bc78911a3672b0cbb5180189460cd9c728dd11a9f71194d1ac6c012102b4a32c64f108fa46fd75a31315e17ffbc8a8ea9a03bd3c7e7229e93e8f72d44402483045022100de1732ee1350d171012fc097af03e91ff26f00cbd2c96e7a0fe2a6abb61b9d1d02206329366501086ef5d2143877eed4c6f6ed93db8ed26c27f360db1b2c55b80156012103715ee32a6095ff88309b22198dc563d4b5e1d7f3387abfc3a643db62252292df024730440220500b1db89395d62c5049414adc52e79d0b5527180669ac811ad61963d57ebec4022000d64181fe2b1c9d0adb2672ad0612c40c4e0cb027e22e340d35a92307ef4c8e012103840f78046760b9024f52aeb59c5a0c36b54ce1551a48427033b95970f1b214f500000000
  assertEquals ${final_transaction} \
    "$(${KEYTOOL} --asset btct --psbt ${psbt_to_sign} --address-ec-key-wif ${key1} psbt-signed \
      | ${KEYTOOL} --asset btct --psbt '' --address-ec-key-wif ${key2} psbt-signed \
      | ${KEYTOOL} --asset btct --psbt '' --address-ec-key-wif ${key3} psbt-signed \
      | ${KEYTOOL} --psbt '' transaction)"
)

testPSBTHex()
(
  psbt_hex=70736274ff0100750200000001268171371edff285e937adeea4b37b78000c0566cbb3ad64641713ca42171bf60000000000feffffff02d3dff505000000001976a914d0c59903c5bac2868760e90fd521a4665aa7652088ac00e1f5050000000017a9143545e6e33b832c47050f24d3eeb93c9c03948bc787b32e1300000100fda5010100000000010289a3c71eab4d20e0371bbba4cc698fa295c9463afa2e397f8533ccb62f9567e50100000017160014be18d152a9b012039daf3da7de4f53349eecb985ffffffff86f8aa43a71dff1448893a530a7237ef6b4608bbb2dd2d0171e63aec6a4890b40100000017160014fe3e9ef1a745e974d902c4355943abcb34bd5353ffffffff0200c2eb0b000000001976a91485cff1097fd9e008bb34af709c62197b38978a4888ac72fef84e2c00000017a914339725ba21efd62ac753a9bcd067d6c7a6a39d05870247304402202712be22e0270f394f568311dc7ca9a68970b8025fdd3b240229f07f8a5f3a240220018b38d7dcd314e734c9276bd6fb40f673325bc4baa144c800d2f2f02db2765c012103d2e15674941bad4a996372cb87e1856d3652606d98562fe39c5e9e7e413f210502483045022100d12b852d85dcd961d2f5f4ab660654df6eedcc794c0c33ce5cc309ffb5fce58d022067338a8e0e1725c197fb1a88af59f51e44e4255b20167c8684031c05d1f2592a01210223b72beef0965d10be0778efecd61fcac6f79a4ea169393380734464f84f2ab300000000000000
  psbt_base64=cHNidP8BAHUCAAAAASaBcTce3/KF6Tet7qSze3gADAVmy7OtZGQXE8pCFxv2AAAAAAD+////AtPf9QUAAAAAGXapFNDFmQPFusKGh2DpD9UhpGZap2UgiKwA4fUFAAAAABepFDVF5uM7gyxHBQ8k0+65PJwDlIvHh7MuEwAAAQD9pQEBAAAAAAECiaPHHqtNIOA3G7ukzGmPopXJRjr6Ljl/hTPMti+VZ+UBAAAAFxYAFL4Y0VKpsBIDna89p95PUzSe7LmF/////4b4qkOnHf8USIk6UwpyN+9rRgi7st0tAXHmOuxqSJC0AQAAABcWABT+Pp7xp0XpdNkCxDVZQ6vLNL1TU/////8CAMLrCwAAAAAZdqkUhc/xCX/Z4Ai7NK9wnGIZeziXikiIrHL++E4sAAAAF6kUM5cluiHv1irHU6m80GfWx6ajnQWHAkcwRAIgJxK+IuAnDzlPVoMR3HyppolwuAJf3TskAinwf4pfOiQCIAGLONfc0xTnNMkna9b7QPZzMlvEuqFEyADS8vAtsnZcASED0uFWdJQbrUqZY3LLh+GFbTZSYG2YVi/jnF6efkE/IQUCSDBFAiEA0SuFLYXc2WHS9fSrZgZU327tzHlMDDPOXMMJ/7X85Y0CIGczio4OFyXBl/saiK9Z9R5E5CVbIBZ8hoQDHAXR8lkqASECI7cr7vCWXRC+B3jv7NYfysb3mk6haTkzgHNEZPhPKrMAAAAAAAAA
  assertEquals ${psbt_base64} \
    "$(${KEYTOOL} --psbt ${psbt_hex} psbt)"
  assertEquals ${psbt_hex} \
    "$(${KEYTOOL} --psbt ${psbt_base64} psbt-hex)"
)

testPSBTUR()
(
  # Test vector from: https://github.com/BlockchainCommons/Research/blob/master/papers/bcr-2020-006-urtypes.md#exampletest-vector-3
  psbt_ur=ur:crypto-psbt/hdosjojkidjyzmadaenyaoaeaeaeaohdvsknclrejnpebncnrnmnjojofejzeojlkerdonspkpkkdkykfelokgprpyutkpaeaeaeaeaezmzmzmzmlslgaaditiwpihbkispkfgrkbdaslewdfycprtjsprsgksecdratkkhktikewdcaadaeaeaeaezmzmzmzmaojopkwtayaeaeaeaecmaebbtphhdnjstiambdassoloimwmlyhygdnlcatnbggtaevyykahaeaeaeaecmaebbaeplptoevwwtyakoonlourgofgvsjydpcaltaemyaeaeaeaeaeaeaeaeaebkgdcarh
  psbt_hex=70736274ff01009a020000000258e87a21b56daf0c23be8e7070456c336f7cbaa5c8757924f545887bb2abdd750000000000ffffffff838d0427d0ec650a68aa46bb0b098aea4422c071b2ca78352a077959d07cea1d0100000000ffffffff0270aaf00800000000160014d85c2b71d0060b09c9886aeb815e50991dda124d00e1f5050000000016001400aea9a2e5f0f876a588df5546e8742d1d87008f000000000000000000
  assertEquals ${psbt_hex} \
    "$(${KEYTOOL} --psbt ${psbt_ur} psbt-hex)"
  assertEquals ${psbt_ur} \
    "$(${KEYTOOL} --psbt ${psbt_hex} psbt-ur)"
)

testTXUR()
(
  transaction_hex=0200000000010258e87a21b56daf0c23be8e7070456c336f7cbaa5c8757924f545887bb2abdd7500000000da00473044022074018ad4180097b873323c0015720b3684cc8123891048e7dbcd9b55ad679c99022073d369b740e3eb53dcefa33823c8070514ca55a7dd9544f157c167913261118c01483045022100f61038b308dc1da865a34852746f015772934208c6d24454393cd99bdf2217770220056e675a675a6d0a02b85b14e5e29074d8a25a9b5760bea2816f661910a006ea01475221029583bf39ae0a609747ad199addd634fa6108559d6c5cd39b4c2183f1ab96e07f2102dab61ff49a14db6a7d02b0cd1fbb78fc4b18312b5b4e54dae4dba2fbfef536d752aeffffffff838d0427d0ec650a68aa46bb0b098aea4422c071b2ca78352a077959d07cea1d01000000232200208c2353173743b595dfb4a07b72ba8e42e3797da74e87fe7d9d7497e3b2028903ffffffff0270aaf00800000000160014d85c2b71d0060b09c9886aeb815e50991dda124d00e1f5050000000016001400aea9a2e5f0f876a588df5546e8742d1d87008f000400473044022062eb7a556107a7c73f45ac4ab5a1dddf6f7075fb1275969a7f383efff784bcb202200c05dbb7470dbf2f08557dd356c7325c1ed30913e996cd3840945db12228da5f01473044022065f45ba5998b59a27ffe1a7bed016af1f1f90d54b3aa8f7450aa5f56a25103bd02207f724703ad1edb96680b284b56d4ffcb88f7fb759eabbe08aa30f29b851383d20147522103089dc10c7ac6db54f91329af617333db388cead0c231f723379d1b99030b02dc21023add904f3d6dcf59ddb906b0dee23529b7ffb9ed50e5e86151926860221f0e7352ae00000000
  transaction_ur=ur:crypto-tx/hkaojyaoaeaeaeaeadaohdvsknclrejnpebncnrnmnjojofejzeojlkerdonspkpkkdkykfelokgprpyutkpaeaeaeaetnaefldyfyaocxjyadletycsaemsrojkeyfnaebzjpbdenlrsflycnldbefdvduysnndgopmionsnlaocxjkteinrlfzvlwmguuowsotetcnspatahbbsggoosutmdfywnhgseiomeeyhsbylkadfddyfeaoclaeynbeetqdayuocapdihotfdgmjyjladhgjpmufwayswtdfyghesfntandurcpchktaocxahjtiohtiohtjnbkaorohpbbvwvomhjytpoehtndhghnrnoelyjliycfbenbamwdadflgmclaomdlsrsesplbkhnmsflpmcfnyuttbeezshsaygontjzhhtendgscllswnpymtvtlbclaotnrpctwknybbuyimkiaopfsnctrkksztgrcsehdnhpglghtnveuyoezozeykentsgmplzmzmzmzmlslgaaditiwpihbkispkfgrkbdaslewdfycprtjsprsgksecdratkkhktikewdcaadaeaeaecncpaecxlkcnguchemfxremdurqznbkgjprdmnfwvlkkkiosglltzekintjymsvlpraoldaxzmzmzmzmaojopkwtayaeaeaeaecmaebbtphhdnjstiambdassoloimwmlyhygdnlcatnbggtaevyykahaeaeaeaecmaebbaeplptoevwwtyakoonlourgofgvsjydpcaltaemyaeaaaefldyfyaocxidwmkngohsatosstfhfepsgereoyuturjljokpzobgkpmtnylbetfmzmyllrrfpraocxbnahuyrlflbtrsdlaygokitehfsteyhhckteasbwwlmtsnetfzmwhlpacpdetnheadfldyfyaocxihwkhponnlluhkoelbzecykgweadimwnwnytbtghqdpkmyjygdpkhehfoegyaxryaocxlbjpflaxpmckuymtisbddegrhftyzmsbloylzokpnnpyrnaypkdywzndlpbwlstdadflgmclaxayntsebnknswuyghytbwdtpehsjkeouyetlkwdtisaehylcnemntcwnlaxbdaouoclaoftutmhgwfsjntkhkutrhampfuevoecdtrlzmrhwegdvwvshsgymoishncpctbajkgmplaeaeaeaedlrfdkro
  assertEquals ${transaction_ur} \
    "$(${KEYTOOL} --transaction ${transaction_hex} transaction-ur)"
  assertEquals ${transaction_hex} \
    "$(${KEYTOOL} --transaction ${transaction_ur} transaction)"
)

# Eat all command-line arguments before calling shunit2.
shift $#
if [ "$(uname)" == "Darwin" ]; then
. shunit2
elif [ "$(uname)" == "Linux" ]; then
# Linux
. shunit2
# No shunit2 on windows/msys2/mingw64 platform
fi
