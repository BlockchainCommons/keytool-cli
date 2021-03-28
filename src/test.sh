#!/bin/bash

# Echo any provided arguments.
# [ $# -gt 0 ] && echo "#:$# 1:$1 2:$2 3:$3"

KEYTOOL=${1:-keytool}

TEST_SEED_HEX='59f2293a5bce7d4de59e71b4207ac5d2'
TEST_SEED_UR='ur:crypto-seed/oyadgdhkwzdtfthptokigtvwnnjsqzcxknsktdhpyljeda'
TEST_SEED_NAME='Dark Purple Aqua Love'
TEST_SEED_NOTE='This is a seed note.'
TEST_SEED_METADATA_UR='ur:crypto-seed/otadgdhkwzdtfthptokigtvwnnjsqzcxknsktdaxkpfyhsjpjecxgdkpjpjojzihcxfpjskphscxgsjlkoihaajyghisinjkcxinjkcxhscxjkihihiecxjtjljyihdmknzsnbwy'
TEST_SEED_DIGEST='ffa11a8b90954fc89ae625779ca11b8f0227573a2f8b4ed85d96ddf901a72cea'
TEST_REQUEST_ID='abca5901-7419-4a37-9473-a55f434ebc27'
TEST_REQUEST_DESC='Information about this request.'
TEST_SEED_REQUEST_UR='ur:crypto-request/otadtpdagdpysghkadjycfgeemmwjkonhefxglrfdiaotaadwkhdcxzmoycylumhmdgwspnyvadaktnsoycwmyaodihgftdllugltphlmtutytadosdwwdaxksctgajtiyjljpjnhsjyinjljtcxhsidjlkpjycxjyisinjkcxjpihjskpihjkjydmetdriyes'
TEST_SEED_RESPONSE_UR='ur:crypto-response/oeadtpdagdpysghkadjycfgeemmwjkonhefxglrfdiaotaaddwoyadgdhkwzdtfthptokigtvwnnjsqzcxknsktdhltklans'
TEST_SEED_RESPONSE_METADATA_UR='ur:crypto-response/oeadtpdagdpysghkadjycfgeemmwjkonhefxglrfdiaotaaddwotadgdhkwzdtfthptokigtvwnnjsqzcxknsktdaxkpfyhsjpjecxgdkpjpjojzihcxfpjskphscxgsjlkoihaajyghisinjkcxinjkcxhscxjkihihiecxjtjljyihdmjliatbcn'
TEST_MASTER_KEY_UR='ur:crypto-hdkey/oxadykaoykaxhdclaeeheyceykguhkgdbzpssfkplpftwdhkaetdetpfcnztjzctlrcygoiobtteplqdwnaahdcxvdbggasekkuytykkfefdlahtflkecpbzyackrpfmghvyqdssfsfhmtzcttsehdcsmusabnke'
TEST_MASTER_KEY_BASE58='xprv9s21ZrQH143K4Mnjc7E8rpSMf8JB1XWmojYf7Ndk6zcNSbUYBsvTqJcdzTok1XwYcgytn5CRxtwhHu93NNXNQwGUbBqL3AHHZZrtKpEvmww'
TEST_MASTER_KEY_FINGERPRINT='604b93f2'
TEST_ACCOUNT_DERIVATION_PATH='84h/0h/0h'
TEST_ACCOUNT_KEY_UR='ur:crypto-hdkey/onaoykaxhdclaejnsodtmtfwdkknwpbeqdlacskoadsgdlehbwrpahdmmepkbywsmuvynlmwbbdktsaahdcxhnfgnepefxgdytryckticelyotsstoknfntavevaskiddmolsarntykbrybtjpksamoeadlncsghykaeykaeykaocyhngrmuwzaycyzssajpsnknaewnko'
TEST_ADDRESS_DERIVATION_PATH='0/0'
TEST_ACCOUNT_PUB_KEY_UR='ur:crypto-hdkey/oxaxhdclaxvlcprfttldjobkredtlnhsidwybaeyjtswyandlgjnehtkdsidbkqzsrkphyfhsaaahdcxhnfgnepefxgdytryckticelyotsstoknfntavevaskiddmolsarntykbrybtjpksamoeadlncsghykaeykaeykaocyhngrmuwzaycyzssajpsndrvlrlbk'
TEST_ADDRESS_KEY_UR='ur:crypto-hdkey/onaoykaxhdclaefwmwpdrhbapygdplnbfwzmmwknglmtneaeurlakggwetenpeiedmbtgwvtmtvtasaahdcxeooeaedscmfdlotnmyvyjzroltoecfglgrjlldbgflkggsayaockrpveamnladfnamoeadlecsghykaeykaeykaewkaewkaocyhngrmuwzaycycyceptprzokgdwim'
TEST_ADDRESS_PUB_KEY_UR='ur:crypto-hdkey/oxaxhdclaolujplrprwzqzcfvymyosglflwkdnyldslrlnqdlnsfistpfwgdheimrscpbtlolnaahdcxeooeaedscmfdlotnmyvyjzroltoecfglgrjlldbgflkggsayaockrpveamnladfnamoeadlecsghykaeykaeykaewkaewkaocyhngrmuwzaycycyceptprfnfewdtd'
TEST_ADDRESS_EC_KEY='4294a8b90eab50aea042ff947a4e969f00df807b4f3836af642e0d4fe096e009'
TEST_ADDRESS_EC_KEY_WIF='KyT8nbdFcaujJ7gE2sL74goxG6wENA7myKDxssEeuK4t93hmsiJ6'
TEST_ADDRESS_PUB_EC_KEY='028b7284b2f2b419e18fa74e47f42bf7268486b386cc68d842505f6abf220d8886'

testDefaults()
{
  # asset
  assertEquals $'btc' "$(${KEYTOOL} asset)"

  # asset
  assertEquals $'mainnet' "$(${KEYTOOL} network)"

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
  # seed-hex <- hex
  assertEquals ${TEST_SEED_HEX} \
    "$(${KEYTOOL} --seed-hex ${TEST_SEED_HEX} seed-hex)"
}

testSeedUROutput()
{
  # seed <- seed-hex
  assertEquals ${TEST_SEED_UR} \
    "$(${KEYTOOL} --seed-hex ${TEST_SEED_HEX} seed)"
}

testSeedURInput()
{
  # seed-hex <- seed
  assertEquals ${TEST_SEED_HEX} \
    "$(${KEYTOOL} --seed ${TEST_SEED_UR} seed-hex)"
}

testReadSeedURMetadata()
{
  assertEquals ${TEST_SEED_HEX}$'\n'"${TEST_SEED_NAME}"$'\n'"${TEST_SEED_NOTE}" \
    "$(${KEYTOOL} --seed ${TEST_SEED_METADATA_UR} seed-hex seed-name seed-note)"
}

testWriteSeedURMetadata()
{
  assertEquals ${TEST_SEED_METADATA_UR} \
    "$(${KEYTOOL} --seed-hex ${TEST_SEED_HEX} --seed-name "${TEST_SEED_NAME}" --seed-note "${TEST_SEED_NOTE}" seed)"
}

testSeedDigest()
{
  assertEquals ${TEST_SEED_DIGEST} \
    "$(${KEYTOOL} --seed-hex ${TEST_SEED_HEX} seed-digest)"
}

testRequestID()
{
  assertEquals ${TEST_REQUEST_ID} \
    "$(${KEYTOOL} --seed-request-id ${TEST_REQUEST_ID} seed-request-id)"
}

testRequestDescription()
{
  assertEquals "${TEST_REQUEST_DESC}" \
    "$(${KEYTOOL} --seed-request-description "${TEST_REQUEST_DESC}" seed-request-description)"
}

testSeedRequest()
{
  assertEquals ${TEST_SEED_REQUEST_UR} \
    "$(${KEYTOOL} --seed-request-id ${TEST_REQUEST_ID} --seed-hex ${TEST_SEED_HEX} --seed-request-description "${TEST_REQUEST_DESC}" seed-request)"
}

testReadSeedRequest()
{
  assertEquals "${TEST_REQUEST_ID}"$'\n'"${TEST_REQUEST_DESC}"$'\n'"${TEST_SEED_DIGEST}" \
    "$(${KEYTOOL} --seed-request ${TEST_SEED_REQUEST_UR} seed-request-id seed-request-description seed-digest)"
}

testSeedResponse1()
{
  assertEquals ${TEST_SEED_RESPONSE_UR} \
    "$(${KEYTOOL} --seed-request-id ${TEST_REQUEST_ID} --seed-hex ${TEST_SEED_HEX} seed-response)"
}

testSeedResponse2()
{
  assertEquals ${TEST_SEED_RESPONSE_METADATA_UR} \
    "$(${KEYTOOL} --seed-request ${TEST_SEED_REQUEST_UR} \
  --seed ${TEST_SEED_METADATA_UR} \
  seed-response)"
}

testReadSeedResponse()
{
  assertEquals "${TEST_SEED_METADATA_UR}"$'\n'"${TEST_REQUEST_ID}"$'\n'"${TEST_SEED_HEX}"$'\n'"${TEST_SEED_NAME}"$'\n'"${TEST_SEED_NOTE}" \
    "$(${KEYTOOL} --seed-response ${TEST_SEED_RESPONSE_METADATA_UR} \
    seed seed-request-id seed-hex seed-name seed-note)"
}

testMasterKey()
{
  # master-key <- [seed-hex, asset, network]
  assertEquals ${TEST_MASTER_KEY_UR} \
    "$(${KEYTOOL} --seed-hex ${TEST_SEED_HEX} master-key)"
}

testMasterKeyUR()
{
  assertEquals ${TEST_MASTER_KEY_UR} \
    "$(${KEYTOOL} --master-key ${TEST_MASTER_KEY_UR} master-key)"
}

testMasterKeyBase58()
{
  # master-key-base58 <- [master-key]
  assertEquals ${TEST_MASTER_KEY_BASE58} \
    "$(${KEYTOOL} --master-key ${TEST_MASTER_KEY_UR} master-key-base58)"

  # master-key <- [master-key-base58]
  assertEquals ${TEST_MASTER_KEY_UR} \
    "$(${KEYTOOL} --master-key-base58 ${TEST_MASTER_KEY_BASE58} master-key)"
}

testMasterKeyFingerprint()
{
  # master-key-fingerprint <- [master-key]
  assertEquals ${TEST_MASTER_KEY_FINGERPRINT} \
    "$(${KEYTOOL} --master-key ${TEST_MASTER_KEY_UR} master-key-fingerprint)"
}

testAccountDerivationPath()
{
  # account-derivation-path <- [master-key-fingerprint, purpose, coin-type, account-index]
  assertEquals ${TEST_ACCOUNT_DERIVATION_PATH} "$(${KEYTOOL} account-derivation-path)"
  assertEquals "9abcdef0/${TEST_ACCOUNT_DERIVATION_PATH}" "$(${KEYTOOL} --master-key-fingerprint 9abcdef0 account-derivation-path)"
  assertEquals "${TEST_MASTER_KEY_FINGERPRINT}/${TEST_ACCOUNT_DERIVATION_PATH}" "$(${KEYTOOL} --master-key ${TEST_MASTER_KEY_UR} account-derivation-path)"
}

testAccountKey()
{
  # account-key <- [master-key, account-derivation-path]
  assertEquals ${TEST_ACCOUNT_KEY_UR} \
    "$(${KEYTOOL} --master-key ${TEST_MASTER_KEY_UR} account-key)"
}

TEST_ACCOUNT_KEY_MINIMAL_UR='ur:crypto-hdkey/onaoykaxhdclaejnsodtmtfwdkknwpbeqdlacskoadsgdlehbwrpahdmmepkbywsmuvynlmwbbdktsaahdcxhnfgnepefxgdytryckticelyotsstoknfntavevaskiddmolsarntykbrybtjpksamoeadlfaeykaxaxaycyzssajpsnyaaemeso'
TEST_ACCOUNT_KEY_BASE58='xprv9zWKRdxk5S4PWnZwz7qyUzFUA86bHXDw6u1vZ9sGPUkknsQU3oE8TBmnEmCtKh9yVjoZySwhUqPSSPzRKt4FuKetSmJGfGoS1YbE3eyyE6N'

testAccountKeyBase58()
{
  # account-key-base58 <- [account-key]
  assertEquals ${TEST_ACCOUNT_KEY_BASE58} \
    "$(${KEYTOOL} --account-key ${TEST_ACCOUNT_KEY_UR} account-key-base58)"

  # account-key-base58 <- [account-key]
  assertEquals ${TEST_ACCOUNT_KEY_BASE58} \
    "$(${KEYTOOL} --account-key ${TEST_ACCOUNT_KEY_MINIMAL_UR} account-key-base58)"

  # account-key <- [account-key-base58]
  assertEquals ${TEST_ACCOUNT_KEY_MINIMAL_UR} \
    "$(${KEYTOOL} --account-key-base58 ${TEST_ACCOUNT_KEY_BASE58} account-key)"
}

testPartialAddressDerivationPath()
{
    # address-derivation-path <- [chain-type, address-index]
    assertEquals ${TEST_ADDRESS_DERIVATION_PATH} "$(${KEYTOOL} address-derivation-path)"
    assertEquals $'1/*' "$(${KEYTOOL} --chain-type change --address-index '*' address-derivation-path)"
}

testFullAddressDerivationPath()
{
  # full-address-derivation-path <- [account-derivation-path, address-derivation-path]
  assertEquals "${TEST_ACCOUNT_DERIVATION_PATH}/${TEST_ADDRESS_DERIVATION_PATH}" "$(${KEYTOOL} full-address-derivation-path)"
}

testAccountPubKey()
{
  # account-pub-key <- [account-key]
  assertEquals ${TEST_ACCOUNT_PUB_KEY_UR} \
    "$(${KEYTOOL} --account-key ${TEST_ACCOUNT_KEY_UR} account-pub-key)"
}

TEST_ACCOUNT_PUB_KEY_MINIMAL_UR='ur:crypto-hdkey/oxaxhdclaxvlcprfttldjobkredtlnhsidwybaeyjtswyandlgjnehtkdsidbkqzsrkphyfhsaaahdcxhnfgnepefxgdytryckticelyotsstoknfntavevaskiddmolsarntykbrybtjpksamoeadlfaeykaxaxaycyzssajpsnbkrdlymo'
TEST_ACCOUNT_PUB_KEY_BASE58='xpub6DVfq9VduocgjGeR69Nyr8CCi9w5gywnU7wXMYGswpHjffjcbLYNzz6G6555VDcSZLDwZPzJHJQabVWWgkpvYntpunL3UjHGrkCJ6VndbQf'

testAccountPubKeyBase58()
{
  # account-pub-key-base58 <- [account-pub-key]
  assertEquals ${TEST_ACCOUNT_PUB_KEY_BASE58} \
    "$(${KEYTOOL} --account-pub-key ${TEST_ACCOUNT_PUB_KEY_UR} account-pub-key-base58)"

  # account-pub-key-base58 <- [account-pub-key]
  assertEquals ${TEST_ACCOUNT_PUB_KEY_BASE58} \
    "$(${KEYTOOL} --account-pub-key ${TEST_ACCOUNT_PUB_KEY_MINIMAL_UR} account-pub-key-base58)"

  # account-pub-key <- [account-pub-key-base58]
  assertEquals ${TEST_ACCOUNT_PUB_KEY_MINIMAL_UR} \
    "$(${KEYTOOL} --account-pub-key-base58 ${TEST_ACCOUNT_PUB_KEY_BASE58} account-pub-key)"
}

testAddressKey()
{
  # address-key <- [master-key, full-address-derivation-path]
  assertEquals ${TEST_ADDRESS_KEY_UR} \
    "$(${KEYTOOL} --master-key ${TEST_MASTER_KEY_UR} address-key)"

  assertEquals ${TEST_ACCOUNT_KEY_UR} \
    "$(${KEYTOOL} --master-key ${TEST_MASTER_KEY_UR} account-key)"

  # address-key <- [account-key, address-derivation-path]
  assertEquals ${TEST_ADDRESS_KEY_UR} \
    "$(${KEYTOOL} --account-key ${TEST_ACCOUNT_KEY_UR} address-key)"
}

TEST_ADDRESS_KEY_BASE58='xprvA2cpWAfRGtwAfZYb3gQ7aDw5H34a8kJszXZD6zXLtXsG58Mn3F3GPJZvD66qfFkmEzks4TuWbenCp9Q5oGx6aCRE4XBg7TMsq56XxjgbN2d'
TEST_ADDRESS_KEY_MINIMAL_UR='ur:crypto-hdkey/onaoykaxhdclaefwmwpdrhbapygdplnbfwzmmwknglmtneaeurlakggwetenpeiedmbtgwvtmtvtasaahdcxeooeaedscmfdlotnmyvyjzroltoecfglgrjlldbgflkggsayaockrpveamnladfnamoeadlfaewkaxahaycycyceptprahosjsbz'

testAddressKeyBase58()
{
  # address-key-base58 <- [address-key]
  assertEquals ${TEST_ADDRESS_KEY_BASE58} \
    "$(${KEYTOOL} --address-key ${TEST_ADDRESS_KEY_UR} address-key-base58)"

  # address-key-base58 <- [address-key]
  assertEquals ${TEST_ADDRESS_KEY_BASE58} \
    "$(${KEYTOOL} --address-key ${TEST_ADDRESS_KEY_MINIMAL_UR} address-key-base58)"

  # address-key <- [address-key-base58]
  assertEquals ${TEST_ADDRESS_KEY_MINIMAL_UR} \
    "$(${KEYTOOL} --address-key-base58 ${TEST_ADDRESS_KEY_BASE58} address-key)"
}

testAddressPubKey()
{
  # address-pub-key <- [address-key]
  assertEquals ${TEST_ADDRESS_PUB_KEY_UR} \
    "$(${KEYTOOL} --address-key ${TEST_ADDRESS_KEY_UR} address-pub-key)"

  # address-pub-key <- [account-pub-key, address-derivation-path]
  assertEquals ${TEST_ADDRESS_PUB_KEY_UR} \
    "$(${KEYTOOL} --account-pub-key ${TEST_ACCOUNT_PUB_KEY_UR} address-pub-key)"
}

TEST_ADDRESS_PUB_KEY_BASE58='xpub6FcAugCK7GVTt3d49hw7wMsoq4u4YD2jMkUouNvxSsQEwvgvanMWw6tQ4MghPAHedPshS8Nf7MdKuFbmrTBV4MQwYTqrbYYzVb1SWify2Kq'
TEST_ADDRESS_PUB_KEY_MINIMAL_UR='ur:crypto-hdkey/oxaxhdclaolujplrprwzqzcfvymyosglflwkdnyldslrlnqdlnsfistpfwgdheimrscpbtlolnaahdcxeooeaedscmfdlotnmyvyjzroltoecfglgrjlldbgflkggsayaockrpveamnladfnamoeadlfaewkaxahaycycyceptpriodigymw'

testAddressPubKeyBase58()
{
  # address-pub-key-base58 <- [address-pub-key]
  assertEquals ${TEST_ADDRESS_PUB_KEY_BASE58} \
    "$(${KEYTOOL} --address-pub-key ${TEST_ADDRESS_PUB_KEY_UR} address-pub-key-base58)"

  # address-pub-key-base58 <- [address-pub-key]
  assertEquals ${TEST_ADDRESS_PUB_KEY_BASE58} \
    "$(${KEYTOOL} --address-pub-key ${TEST_ADDRESS_PUB_KEY_MINIMAL_UR} address-pub-key-base58)"

  # address-pub-key <- [address-pub-key-base58]
  assertEquals ${TEST_ADDRESS_PUB_KEY_MINIMAL_UR} \
    "$(${KEYTOOL} --address-pub-key-base58 ${TEST_ADDRESS_PUB_KEY_BASE58} address-pub-key)"
}

testAddressECKey1()
{
  # address-ec-key <- [address-key]
  assertEquals ${TEST_ADDRESS_EC_KEY} \
    "$(${KEYTOOL} --address-key ${TEST_ADDRESS_KEY_UR} address-ec-key)"
}

testAddressECKeyWIF()
{
  # address-ec-key-wif <- [address-ec-key, network]
  assertEquals ${TEST_ADDRESS_EC_KEY_WIF} \
    "$(${KEYTOOL} --address-ec-key ${TEST_ADDRESS_EC_KEY} address-ec-key-wif)"

  assertEquals ${TEST_ADDRESS_EC_KEY_WIF} \
    "$(${KEYTOOL} --address-key ${TEST_ADDRESS_KEY_UR} address-ec-key-wif)"
}

testAddressECKey2()
{
  # address-ec-key <- [address-ec-key-wif]
  assertEquals ${TEST_ADDRESS_EC_KEY} \
    "$(${KEYTOOL} --address-ec-key-wif ${TEST_ADDRESS_EC_KEY_WIF} address-ec-key)"
}

testAddressPubECKey()
{
  # address-pub-ec-key <- [address-ec-key]
  assertEquals ${TEST_ADDRESS_PUB_EC_KEY} \
    "$(${KEYTOOL} --address-ec-key ${TEST_ADDRESS_EC_KEY} address-pub-ec-key)"

  # address-pub-ec-key <- [address-pub-key]
  assertEquals ${TEST_ADDRESS_PUB_EC_KEY} \
    "$(${KEYTOOL} --address-pub-key ${TEST_ADDRESS_PUB_KEY_UR} address-pub-ec-key)"
}

testAddressPKH()
{
  # address-pkh <- [address-pub-ec-key, asset, network]
  assertEquals $'1Ah3MPVCRuAG6M7kGU8fU1Aze3dBYuTF3f' \
    "$(${KEYTOOL} --address-pub-ec-key ${TEST_ADDRESS_PUB_EC_KEY} address-pkh)"
}

testAddressSH()
{
  # address-sh <- [address-pub-ec-key, asset, network]
  assertEquals $'3BP4GvydyoUeBWpBPZoFtdXvnZuu8Q9mVG' \
    "$(${KEYTOOL} --address-pub-ec-key ${TEST_ADDRESS_PUB_EC_KEY} address-sh)"
}

testOutputDescriptor()
{
  # output-descriptor <- [output-descriptor-type, account-derivation-path, address-derivation-path, account-pub-key]
  assertEquals $'sh(wpkh([604b93f2/49h/0h/0h]xpub6CiwBquyrgYoKFuQRxvp2EbJBVd6HfSoceSj57m3XTLaMLsyAvKZXLu9kr1ocV3LFYDcNd1wADSaCurZowTwpBSko3BrRrAoNk6m3or7btT/0/*))' \
    "$(${KEYTOOL} --seed-hex ${TEST_SEED_HEX} --output-type sh-wpkh --address-index '*' output-descriptor)"
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
    "$(${KEYTOOL} --network testnet --master-key-base58 ${master_key} --full-address-derivation-path m/0h/0h/0h address-ec-key-wif)"

  assertEquals $key2 \
    "$(${KEYTOOL} --network testnet --master-key-base58 ${master_key} --full-address-derivation-path m/0h/0h/2h address-ec-key-wif)"
)

testPSBTSigning()
(
  psbt_to_sign=cHNidP8BAMMCAAAAA0DcshmpIS/ZW94f7roto1MhBNhk6Rz/ZaetRRfcedh9AAAAAAD9////wm+mbFIbTYiPlNNhB8P1/vBcwKG/ZryOHMKkL7dSqTYAAAAAAP3///+CmZgUyFrdKNCZUoqfgBhCAFORPdiP/qr0OOt4I5n+CgAAAAAA/f///wJAQg8AAAAAABYAFCBk78yn2RApIqWfuXigrti3WcQRncYQAAAAAAAWABSMyh/Y85oXJKUZQJT+IeAZJk/MeAAAAAAAAQD9PgECAAAABnPYBYJW735g7YMuaFrrK2ZgQj2XELyHukIJmdvOUB4sAAAAAAD+////bmU42jLXSFq3iLHmsyOR/OrF/x0ZQ2lAbecy8CeanlsAAAAAAP7///87WWkGf++EI4Um41R9YUGj00Z0+wvW0fmPYJ+81lWZOAEAAAAA/v///++syYWL4jBkYbhPlOrxJkozfdkL/XJWnhl2nVzVxa2OAAAAAAD+////UAL64CRRErlwBwQM+X5opqpiI0JmpsKs2a1MzGtZh8QAAAAAAP7///8chEjupyVpNkPdCyBE1wpPrS8MGI9EjEGpj7CTgZAXtgAAAAAA/v///wKghgEAAAAAABYAFMzoxnRhxnGxMSMykm4rhwto1dz6vkMPAAAAAAAWABTzi7rfXv1coXc7HvFo2QoeYq4U1E54HAABAR+ghgEAAAAAABYAFMzoxnRhxnGxMSMykm4rhwto1dz6IgYCtKMsZPEI+kb9daMTFeF/+8io6poDvTx+cinpPo9y1EQYDwVpQ1QAAIABAACAAAAAgAAAAAAQAAAAAAEAUgIAAAABbEc0zoXZ/16+PtvwSQ43BytcwpYvO6gGU11xDcaGH7YAAAAAAAEAAAABgEEPAAAAAAAWABQMEbJbdDyzQi5GH1/cPH4QeuPCiwAAAAABAR+AQQ8AAAAAABYAFAwRslt0PLNCLkYfX9w8fhB648KLIgYDcV7jKmCV/4gwmyIZjcVj1LXh1/M4er/DpkPbYiUikt8YDwVpQ1QAAIABAACAAAAAgAAAAACIAAAAAAEAUgIAAAABtvIk4LNoWJem4tfDI5qeVZKR954vLCVJBiqv3pSFR8AAAAAAAAEAAAAB0kEPAAAAAAAWABT93jySCCHYPSeMHxs0jj9i+0UtmAAAAAABAR/SQQ8AAAAAABYAFP3ePJIIIdg9J4wfGzSOP2L7RS2YIgYDhA94BGdguQJPUq61nFoMNrVM4VUaSEJwM7lZcPGyFPUYDwVpQ1QAAIABAACAAAAAgAAAAACGAAAAACICAgFa3FuEwDkszXFDVxN6KU6pVUV/f6d1JSWZ1RsD4CNtGA8FaUNUAACAAQAAgAAAAIAAAAAAiQAAAAAiAgJZ8CLw/Bdus5hyuOPx7cnFfkBkRoTavnS6BRAHi5GOLBgPBWlDVAAAgAEAAIAAAACAAQAAAFIAAAAA
  key1=cSyLgU8rSvYNU1j6XR2vo5ebSQqza7PR9iFNtkkYrFMwyB5or5gH
  key2=cVok3VJ1fGhdT2yAndKHFWvgTbDsdVFXb8xE9rsAcQASuS4JV9HG
  key3=cMaqfcb16JjrCdfYTfT2vRKFpGjdAugAucEAxw3VuhN91XgjKqzF
  final_transaction=0200000000010340dcb219a9212fd95bde1feeba2da3532104d864e91cff65a7ad4517dc79d87d0000000000fdffffffc26fa66c521b4d888f94d36107c3f5fef05cc0a1bf66bc8e1cc2a42fb752a9360000000000fdffffff82999814c85add28d099528a9f8018420053913dd88ffeaaf438eb782399fe0a0000000000fdffffff0240420f00000000001600142064efcca7d9102922a59fb978a0aed8b759c4119dc61000000000001600148cca1fd8f39a1724a5194094fe21e019264fcc7802483045022100e49b0e63ffba6e684f6971436b38a7b81cba5780fdb0784acd12c145254590600220280aa566aba4bc78911a3672b0cbb5180189460cd9c728dd11a9f71194d1ac6c012102b4a32c64f108fa46fd75a31315e17ffbc8a8ea9a03bd3c7e7229e93e8f72d44402483045022100de1732ee1350d171012fc097af03e91ff26f00cbd2c96e7a0fe2a6abb61b9d1d02206329366501086ef5d2143877eed4c6f6ed93db8ed26c27f360db1b2c55b80156012103715ee32a6095ff88309b22198dc563d4b5e1d7f3387abfc3a643db62252292df024730440220500b1db89395d62c5049414adc52e79d0b5527180669ac811ad61963d57ebec4022000d64181fe2b1c9d0adb2672ad0612c40c4e0cb027e22e340d35a92307ef4c8e012103840f78046760b9024f52aeb59c5a0c36b54ce1551a48427033b95970f1b214f500000000
  assertEquals ${final_transaction} \
    "$(${KEYTOOL} --network testnet --psbt ${psbt_to_sign} --address-ec-key-wif ${key1} psbt-signed \
      | ${KEYTOOL} --network testnet --psbt '' --address-ec-key-wif ${key2} psbt-signed \
      | ${KEYTOOL} --network testnet --psbt '' --address-ec-key-wif ${key3} psbt-signed \
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
