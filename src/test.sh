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

# Eat all command-line arguments before calling shunit2.
shift $#
if [ "$(uname)" == "Darwin" ]; then
. /usr/local/bin/shunit2
elif [ "$(uname)" == "Linux" ]; then
# Linux
. shunit2
# No shunit2 on windows/msys2/mingw64 platform
fi
