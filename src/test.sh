#!/bin/bash

# Echo any provided arguments.
# [ $# -gt 0 ] && echo "#:$# 1:$1 2:$2 3:$3"

KEYTOOL=${1:-keytool}

testDefaults()
{
  # asset
  assertEquals $'btc' "$(${KEYTOOL} asset)"

  # purpose
  assertEquals $'44' "$(${KEYTOOL} purpose)"

  # account-index
  assertEquals $'0' "$(${KEYTOOL} account-index)"

  # address-index
  assertEquals $'0' "$(${KEYTOOL} address-index)"

  # chain-type
  assertEquals $'external' "$(${KEYTOOL} chain-type)"
}

testMultilineOutput()
{
  assertEquals $'btc\n44' "$(${KEYTOOL} asset purpose)"
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

testAccountDerivationPath()
{
  # account-derivation-path <- [purpose, coin-type, account-index]
  assertEquals $'m/44h/0h/0h' "$(${KEYTOOL} account-derivation-path)"
}

testAccountKey()
{
  # account-key <- [master-key, account-derivation-path]
  assertEquals $'xprv9zEdUqPXrhoqBvxoxmTXhh6ieR3iUL3UpBy8ZqeEov5G6ubmphjipJByVhoxBnRtF5LhgrCSyQLEDKLqKoBVJHzDHXZBkhXmfbDje6Bm7AQ' \
    "$(${KEYTOOL} --master-key xprv9s21ZrQH143K28kcT3e8kegkeMexRDzozueBMsHVtctk8gYxLUiBVqzZpK8KwcTjz5xnzZq84ymFKTmbXqwdexKJRkSuejVcCAM8P7sc39b account-key)"
}

testAddressDerivationPath()
{
  # address-derivation-path <- [account-derivation-path, chain-type-int, address-index]
  assertEquals $'m/44h/0h/0h/0/0' "$(${KEYTOOL} address-derivation-path)"
}

testAccountPubKey()
{
  # account-pub-key <- [account-key]
  assertEquals $'xpub6DDytLvRh5N8QR3H4nzY4q3TCStCsnmLBQtjNE3rNFcEyhvvNF3yN6WTLyK599xkUdEKycY5cxcujv9u9YeiENgYBewfCUUHXdQmzj3fjqo' \
    "$(${KEYTOOL} --account-key xprv9zEdUqPXrhoqBvxoxmTXhh6ieR3iUL3UpBy8ZqeEov5G6ubmphjipJByVhoxBnRtF5LhgrCSyQLEDKLqKoBVJHzDHXZBkhXmfbDje6Bm7AQ account-pub-key)"
}

testAddressKey()
{
  # address-key <- [master-key, address-derivation-path]
  assertEquals $'xprvA2p2Wu4w5XUZ6L2iHjvuxCkm38bUcwNCKwSB9XXG5oMjArwtkm2TeA3amg5p73QZVH6owECFo4T9nGnCEuwgUMWLbivB24yyjpsWBWPvYgf' \
    "$(${KEYTOOL} --master-key xprv9s21ZrQH143K28kcT3e8kegkeMexRDzozueBMsHVtctk8gYxLUiBVqzZpK8KwcTjz5xnzZq84ymFKTmbXqwdexKJRkSuejVcCAM8P7sc39b address-key)"

  # address-key <- [account-key, chain-type-int, address-index]
  assertEquals $'xprvA2p2Wu4w5XUZ6L2iHjvuxCkm38bUcwNCKwSB9XXG5oMjArwtkm2TeA3amg5p73QZVH6owECFo4T9nGnCEuwgUMWLbivB24yyjpsWBWPvYgf' \
    "$(${KEYTOOL} --account-key xprv9zEdUqPXrhoqBvxoxmTXhh6ieR3iUL3UpBy8ZqeEov5G6ubmphjipJByVhoxBnRtF5LhgrCSyQLEDKLqKoBVJHzDHXZBkhXmfbDje6Bm7AQ address-key)"
}

testAddressPubKey()
{
  # address-pub-key <- [address-key]
  assertEquals $'xpub6FoNvQbpuu2rJp7BPmTvKLhVbARy2Q63hAMmwuvse8ti3fH3JJLiBxN4cyXjeypWSen5ydkxfxJCLkPydGqQrSdhAwxe9ExFzbakNZ46AK6' \
    "$(${KEYTOOL} --address-key xprvA2p2Wu4w5XUZ6L2iHjvuxCkm38bUcwNCKwSB9XXG5oMjArwtkm2TeA3amg5p73QZVH6owECFo4T9nGnCEuwgUMWLbivB24yyjpsWBWPvYgf address-pub-key)"

  # address-pub-key <- [account-pub-key, chain-type-int, address-index]
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

# Eat all command-line arguments before calling shunit2.
shift $#
. /usr/local/bin/shunit2
