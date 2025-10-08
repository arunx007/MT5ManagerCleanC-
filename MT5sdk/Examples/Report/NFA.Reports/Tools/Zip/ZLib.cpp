//+------------------------------------------------------------------+
//|                                                                  |
//|                             Copyright 2000-2025, MetaQuotes Ltd. |
//|                                               www.metaquotes.net |
//+------------------------------------------------------------------+
//+----------------------------------------------------------------------------+
//| Copyright (C) 1995-2017 Jean-loup Gailly and Mark Adler                    |
//|                                                                            |
//| This software is provided 'as-is', without any express or implied          |
//| warranty.  In no event will the authors be held liable for any damages     |
//| arising from the use of this software.                                     |
//|                                                                            |
//| Permission is granted to anyone to use this software for any purpose,      |
//| including commercial applications, and to alter it and redistribute it     |
//| freely, subject to the following restrictions:                             |
//|                                                                            |
//| 1. The origin of this software must not be misrepresented; you must not    |
//|    claim that you wrote the original software. If you use this software    |
//|    in a product, an acknowledgment in the product documentation would be   |
//|    appreciated but is not required.                                        |
//| 2. Altered source versions must be plainly marked as such, and must not be |
//|    misrepresented as being the original software.                          |
//| 3. This notice may not be removed or altered from any source distribution. |
//|                                                                            |
//| Jean-loup Gailly        Mark Adler                                         |
//| jloup@gzip.org          madler@alumni.caltech.edu                          |
//|                                                                            |
//|                                                                            |
//| The data format used by the zlib library is described by RFCs (Request for |
//| Comments) 1950 to 1952 in the files http://tools.ietf.org/html/rfc1950     |
//| (zlib format), rfc1951 (deflate format) and rfc1952 (gzip format).         |
//+----------------------------------------------------------------------------+
#include "stdafx.h"
#include "Zlib.h"
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
const SZlib::config SZlib::configuration_table[10]=
  {
     {0,    0,  0,     0, deflate_stored},
   {4,    4,  8,     4, deflate_fast},
   {4,    5, 16,     8, deflate_fast},
   {4,    6, 32,    32, deflate_fast},
   {4,    4, 16,    16, deflate_slow},
   {8,   16, 32,    32, deflate_slow},
   {8,   16, 128,  128, deflate_slow},
   {8,   32, 128,  256, deflate_slow},
   {32, 128, 258, 1024, deflate_slow},
   {32, 258, 258, 4096, deflate_slow}
  };
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
const uint32_t SZlib::crc_table[8][256] =
  {
                                                 {
                                                                        0x00000000UL, 0x77073096UL, 0xee0e612cUL, 0x990951baUL, 0x076dc419UL, 0x706af48fUL, 0xe963a535UL, 0x9e6495a3UL, 0x0edb8832UL, 0x79dcb8a4UL, 0xe0d5e91eUL, 0x97d2d988UL, 0x09b64c2bUL, 0x7eb17cbdUL, 0xe7b82d07UL, 0x90bf1d91UL, 0x1db71064UL, 0x6ab020f2UL, 0xf3b97148UL, 0x84be41deUL,
                                                                        0x1adad47dUL, 0x6ddde4ebUL, 0xf4d4b551UL, 0x83d385c7UL, 0x136c9856UL, 0x646ba8c0UL, 0xfd62f97aUL, 0x8a65c9ecUL, 0x14015c4fUL, 0x63066cd9UL, 0xfa0f3d63UL, 0x8d080df5UL, 0x3b6e20c8UL, 0x4c69105eUL, 0xd56041e4UL, 0xa2677172UL, 0x3c03e4d1UL, 0x4b04d447UL, 0xd20d85fdUL, 0xa50ab56bUL,
                                                                        0x35b5a8faUL, 0x42b2986cUL, 0xdbbbc9d6UL, 0xacbcf940UL, 0x32d86ce3UL, 0x45df5c75UL, 0xdcd60dcfUL, 0xabd13d59UL, 0x26d930acUL, 0x51de003aUL, 0xc8d75180UL, 0xbfd06116UL, 0x21b4f4b5UL, 0x56b3c423UL, 0xcfba9599UL, 0xb8bda50fUL, 0x2802b89eUL, 0x5f058808UL, 0xc60cd9b2UL, 0xb10be924UL,
                                                                        0x2f6f7c87UL, 0x58684c11UL, 0xc1611dabUL, 0xb6662d3dUL, 0x76dc4190UL, 0x01db7106UL, 0x98d220bcUL, 0xefd5102aUL, 0x71b18589UL, 0x06b6b51fUL, 0x9fbfe4a5UL, 0xe8b8d433UL, 0x7807c9a2UL, 0x0f00f934UL, 0x9609a88eUL, 0xe10e9818UL, 0x7f6a0dbbUL, 0x086d3d2dUL, 0x91646c97UL, 0xe6635c01UL,
                                                                        0x6b6b51f4UL, 0x1c6c6162UL, 0x856530d8UL, 0xf262004eUL, 0x6c0695edUL, 0x1b01a57bUL, 0x8208f4c1UL, 0xf50fc457UL, 0x65b0d9c6UL, 0x12b7e950UL, 0x8bbeb8eaUL, 0xfcb9887cUL, 0x62dd1ddfUL, 0x15da2d49UL, 0x8cd37cf3UL, 0xfbd44c65UL, 0x4db26158UL, 0x3ab551ceUL, 0xa3bc0074UL, 0xd4bb30e2UL,
                                                                        0x4adfa541UL, 0x3dd895d7UL, 0xa4d1c46dUL, 0xd3d6f4fbUL, 0x4369e96aUL, 0x346ed9fcUL, 0xad678846UL, 0xda60b8d0UL, 0x44042d73UL, 0x33031de5UL, 0xaa0a4c5fUL, 0xdd0d7cc9UL, 0x5005713cUL, 0x270241aaUL, 0xbe0b1010UL, 0xc90c2086UL, 0x5768b525UL, 0x206f85b3UL, 0xb966d409UL, 0xce61e49fUL,
                                                                        0x5edef90eUL, 0x29d9c998UL, 0xb0d09822UL, 0xc7d7a8b4UL, 0x59b33d17UL, 0x2eb40d81UL, 0xb7bd5c3bUL, 0xc0ba6cadUL, 0xedb88320UL, 0x9abfb3b6UL, 0x03b6e20cUL, 0x74b1d29aUL, 0xead54739UL, 0x9dd277afUL, 0x04db2615UL, 0x73dc1683UL, 0xe3630b12UL, 0x94643b84UL, 0x0d6d6a3eUL, 0x7a6a5aa8UL,
                                                                        0xe40ecf0bUL, 0x9309ff9dUL, 0x0a00ae27UL, 0x7d079eb1UL, 0xf00f9344UL, 0x8708a3d2UL, 0x1e01f268UL, 0x6906c2feUL, 0xf762575dUL, 0x806567cbUL, 0x196c3671UL, 0x6e6b06e7UL, 0xfed41b76UL, 0x89d32be0UL, 0x10da7a5aUL, 0x67dd4accUL, 0xf9b9df6fUL, 0x8ebeeff9UL, 0x17b7be43UL, 0x60b08ed5UL,
                                                                        0xd6d6a3e8UL, 0xa1d1937eUL, 0x38d8c2c4UL, 0x4fdff252UL, 0xd1bb67f1UL, 0xa6bc5767UL, 0x3fb506ddUL, 0x48b2364bUL, 0xd80d2bdaUL, 0xaf0a1b4cUL, 0x36034af6UL, 0x41047a60UL, 0xdf60efc3UL, 0xa867df55UL, 0x316e8eefUL, 0x4669be79UL, 0xcb61b38cUL, 0xbc66831aUL, 0x256fd2a0UL, 0x5268e236UL,
                                                                        0xcc0c7795UL, 0xbb0b4703UL, 0x220216b9UL, 0x5505262fUL, 0xc5ba3bbeUL, 0xb2bd0b28UL, 0x2bb45a92UL, 0x5cb36a04UL, 0xc2d7ffa7UL, 0xb5d0cf31UL, 0x2cd99e8bUL, 0x5bdeae1dUL, 0x9b64c2b0UL, 0xec63f226UL, 0x756aa39cUL, 0x026d930aUL, 0x9c0906a9UL, 0xeb0e363fUL, 0x72076785UL, 0x05005713UL,
                                                                        0x95bf4a82UL, 0xe2b87a14UL, 0x7bb12baeUL, 0x0cb61b38UL, 0x92d28e9bUL, 0xe5d5be0dUL, 0x7cdcefb7UL, 0x0bdbdf21UL, 0x86d3d2d4UL, 0xf1d4e242UL, 0x68ddb3f8UL, 0x1fda836eUL, 0x81be16cdUL, 0xf6b9265bUL, 0x6fb077e1UL, 0x18b74777UL, 0x88085ae6UL, 0xff0f6a70UL, 0x66063bcaUL, 0x11010b5cUL,
                                                                        0x8f659effUL, 0xf862ae69UL, 0x616bffd3UL, 0x166ccf45UL, 0xa00ae278UL, 0xd70dd2eeUL, 0x4e048354UL, 0x3903b3c2UL, 0xa7672661UL, 0xd06016f7UL, 0x4969474dUL, 0x3e6e77dbUL, 0xaed16a4aUL, 0xd9d65adcUL, 0x40df0b66UL, 0x37d83bf0UL, 0xa9bcae53UL, 0xdebb9ec5UL, 0x47b2cf7fUL, 0x30b5ffe9UL,
                                                                        0xbdbdf21cUL, 0xcabac28aUL, 0x53b39330UL, 0x24b4a3a6UL, 0xbad03605UL, 0xcdd70693UL, 0x54de5729UL, 0x23d967bfUL, 0xb3667a2eUL, 0xc4614ab8UL, 0x5d681b02UL, 0x2a6f2b94UL, 0xb40bbe37UL, 0xc30c8ea1UL, 0x5a05df1bUL, 0x2d02ef8dUL
      },
     {
      0x00000000UL, 0x191b3141UL, 0x32366282UL, 0x2b2d53c3UL, 0x646cc504UL, 0x7d77f445UL, 0x565aa786UL, 0x4f4196c7UL, 0xc8d98a08UL, 0xd1c2bb49UL, 0xfaefe88aUL, 0xe3f4d9cbUL, 0xacb54f0cUL, 0xb5ae7e4dUL, 0x9e832d8eUL, 0x87981ccfUL, 0x4ac21251UL, 0x53d92310UL, 0x78f470d3UL, 0x61ef4192UL,
      0x2eaed755UL, 0x37b5e614UL, 0x1c98b5d7UL, 0x05838496UL, 0x821b9859UL, 0x9b00a918UL, 0xb02dfadbUL, 0xa936cb9aUL, 0xe6775d5dUL, 0xff6c6c1cUL, 0xd4413fdfUL, 0xcd5a0e9eUL, 0x958424a2UL, 0x8c9f15e3UL, 0xa7b24620UL, 0xbea97761UL, 0xf1e8e1a6UL, 0xe8f3d0e7UL, 0xc3de8324UL, 0xdac5b265UL,
      0x5d5daeaaUL, 0x44469febUL, 0x6f6bcc28UL, 0x7670fd69UL, 0x39316baeUL, 0x202a5aefUL, 0x0b07092cUL, 0x121c386dUL, 0xdf4636f3UL, 0xc65d07b2UL, 0xed705471UL, 0xf46b6530UL, 0xbb2af3f7UL, 0xa231c2b6UL, 0x891c9175UL, 0x9007a034UL, 0x179fbcfbUL, 0x0e848dbaUL, 0x25a9de79UL, 0x3cb2ef38UL,
      0x73f379ffUL, 0x6ae848beUL, 0x41c51b7dUL, 0x58de2a3cUL, 0xf0794f05UL, 0xe9627e44UL, 0xc24f2d87UL, 0xdb541cc6UL, 0x94158a01UL, 0x8d0ebb40UL, 0xa623e883UL, 0xbf38d9c2UL, 0x38a0c50dUL, 0x21bbf44cUL, 0x0a96a78fUL, 0x138d96ceUL, 0x5ccc0009UL, 0x45d73148UL, 0x6efa628bUL, 0x77e153caUL,
      0xbabb5d54UL, 0xa3a06c15UL, 0x888d3fd6UL, 0x91960e97UL, 0xded79850UL, 0xc7cca911UL, 0xece1fad2UL, 0xf5facb93UL, 0x7262d75cUL, 0x6b79e61dUL, 0x4054b5deUL, 0x594f849fUL, 0x160e1258UL, 0x0f152319UL, 0x243870daUL, 0x3d23419bUL, 0x65fd6ba7UL, 0x7ce65ae6UL, 0x57cb0925UL, 0x4ed03864UL,
      0x0191aea3UL, 0x188a9fe2UL, 0x33a7cc21UL, 0x2abcfd60UL, 0xad24e1afUL, 0xb43fd0eeUL, 0x9f12832dUL, 0x8609b26cUL, 0xc94824abUL, 0xd05315eaUL, 0xfb7e4629UL, 0xe2657768UL, 0x2f3f79f6UL, 0x362448b7UL, 0x1d091b74UL, 0x04122a35UL, 0x4b53bcf2UL, 0x52488db3UL, 0x7965de70UL, 0x607eef31UL,
      0xe7e6f3feUL, 0xfefdc2bfUL, 0xd5d0917cUL, 0xcccba03dUL, 0x838a36faUL, 0x9a9107bbUL, 0xb1bc5478UL, 0xa8a76539UL, 0x3b83984bUL, 0x2298a90aUL, 0x09b5fac9UL, 0x10aecb88UL, 0x5fef5d4fUL, 0x46f46c0eUL, 0x6dd93fcdUL, 0x74c20e8cUL, 0xf35a1243UL, 0xea412302UL, 0xc16c70c1UL, 0xd8774180UL,
      0x9736d747UL, 0x8e2de606UL, 0xa500b5c5UL, 0xbc1b8484UL, 0x71418a1aUL, 0x685abb5bUL, 0x4377e898UL, 0x5a6cd9d9UL, 0x152d4f1eUL, 0x0c367e5fUL, 0x271b2d9cUL, 0x3e001cddUL, 0xb9980012UL, 0xa0833153UL, 0x8bae6290UL, 0x92b553d1UL, 0xddf4c516UL, 0xc4eff457UL, 0xefc2a794UL, 0xf6d996d5UL,
      0xae07bce9UL, 0xb71c8da8UL, 0x9c31de6bUL, 0x852aef2aUL, 0xca6b79edUL, 0xd37048acUL, 0xf85d1b6fUL, 0xe1462a2eUL, 0x66de36e1UL, 0x7fc507a0UL, 0x54e85463UL, 0x4df36522UL, 0x02b2f3e5UL, 0x1ba9c2a4UL, 0x30849167UL, 0x299fa026UL, 0xe4c5aeb8UL, 0xfdde9ff9UL, 0xd6f3cc3aUL, 0xcfe8fd7bUL,
      0x80a96bbcUL, 0x99b25afdUL, 0xb29f093eUL, 0xab84387fUL, 0x2c1c24b0UL, 0x350715f1UL, 0x1e2a4632UL, 0x07317773UL, 0x4870e1b4UL, 0x516bd0f5UL, 0x7a468336UL, 0x635db277UL, 0xcbfad74eUL, 0xd2e1e60fUL, 0xf9ccb5ccUL, 0xe0d7848dUL, 0xaf96124aUL, 0xb68d230bUL, 0x9da070c8UL, 0x84bb4189UL,
      0x03235d46UL, 0x1a386c07UL, 0x31153fc4UL, 0x280e0e85UL, 0x674f9842UL, 0x7e54a903UL, 0x5579fac0UL, 0x4c62cb81UL, 0x8138c51fUL, 0x9823f45eUL, 0xb30ea79dUL, 0xaa1596dcUL, 0xe554001bUL, 0xfc4f315aUL, 0xd7626299UL, 0xce7953d8UL, 0x49e14f17UL, 0x50fa7e56UL, 0x7bd72d95UL, 0x62cc1cd4UL,
      0x2d8d8a13UL, 0x3496bb52UL, 0x1fbbe891UL, 0x06a0d9d0UL, 0x5e7ef3ecUL, 0x4765c2adUL, 0x6c48916eUL, 0x7553a02fUL, 0x3a1236e8UL, 0x230907a9UL, 0x0824546aUL, 0x113f652bUL, 0x96a779e4UL, 0x8fbc48a5UL, 0xa4911b66UL, 0xbd8a2a27UL, 0xf2cbbce0UL, 0xebd08da1UL, 0xc0fdde62UL, 0xd9e6ef23UL,
      0x14bce1bdUL, 0x0da7d0fcUL, 0x268a833fUL, 0x3f91b27eUL, 0x70d024b9UL, 0x69cb15f8UL, 0x42e6463bUL, 0x5bfd777aUL, 0xdc656bb5UL, 0xc57e5af4UL, 0xee530937UL, 0xf7483876UL, 0xb809aeb1UL, 0xa1129ff0UL, 0x8a3fcc33UL, 0x9324fd72UL
     },
     {
      0x00000000UL, 0x01c26a37UL, 0x0384d46eUL, 0x0246be59UL, 0x0709a8dcUL, 0x06cbc2ebUL, 0x048d7cb2UL, 0x054f1685UL, 0x0e1351b8UL, 0x0fd13b8fUL, 0x0d9785d6UL, 0x0c55efe1UL, 0x091af964UL, 0x08d89353UL, 0x0a9e2d0aUL, 0x0b5c473dUL, 0x1c26a370UL, 0x1de4c947UL, 0x1fa2771eUL, 0x1e601d29UL,
      0x1b2f0bacUL, 0x1aed619bUL, 0x18abdfc2UL, 0x1969b5f5UL, 0x1235f2c8UL, 0x13f798ffUL, 0x11b126a6UL, 0x10734c91UL, 0x153c5a14UL, 0x14fe3023UL, 0x16b88e7aUL, 0x177ae44dUL, 0x384d46e0UL, 0x398f2cd7UL, 0x3bc9928eUL, 0x3a0bf8b9UL, 0x3f44ee3cUL, 0x3e86840bUL, 0x3cc03a52UL, 0x3d025065UL,
      0x365e1758UL, 0x379c7d6fUL, 0x35dac336UL, 0x3418a901UL, 0x3157bf84UL, 0x3095d5b3UL, 0x32d36beaUL, 0x331101ddUL, 0x246be590UL, 0x25a98fa7UL, 0x27ef31feUL, 0x262d5bc9UL, 0x23624d4cUL, 0x22a0277bUL, 0x20e69922UL, 0x2124f315UL, 0x2a78b428UL, 0x2bbade1fUL, 0x29fc6046UL, 0x283e0a71UL,
      0x2d711cf4UL, 0x2cb376c3UL, 0x2ef5c89aUL, 0x2f37a2adUL, 0x709a8dc0UL, 0x7158e7f7UL, 0x731e59aeUL, 0x72dc3399UL, 0x7793251cUL, 0x76514f2bUL, 0x7417f172UL, 0x75d59b45UL, 0x7e89dc78UL, 0x7f4bb64fUL, 0x7d0d0816UL, 0x7ccf6221UL, 0x798074a4UL, 0x78421e93UL, 0x7a04a0caUL, 0x7bc6cafdUL,
      0x6cbc2eb0UL, 0x6d7e4487UL, 0x6f38fadeUL, 0x6efa90e9UL, 0x6bb5866cUL, 0x6a77ec5bUL, 0x68315202UL, 0x69f33835UL, 0x62af7f08UL, 0x636d153fUL, 0x612bab66UL, 0x60e9c151UL, 0x65a6d7d4UL, 0x6464bde3UL, 0x662203baUL, 0x67e0698dUL, 0x48d7cb20UL, 0x4915a117UL, 0x4b531f4eUL, 0x4a917579UL,
      0x4fde63fcUL, 0x4e1c09cbUL, 0x4c5ab792UL, 0x4d98dda5UL, 0x46c49a98UL, 0x4706f0afUL, 0x45404ef6UL, 0x448224c1UL, 0x41cd3244UL, 0x400f5873UL, 0x4249e62aUL, 0x438b8c1dUL, 0x54f16850UL, 0x55330267UL, 0x5775bc3eUL, 0x56b7d609UL, 0x53f8c08cUL, 0x523aaabbUL, 0x507c14e2UL, 0x51be7ed5UL,
      0x5ae239e8UL, 0x5b2053dfUL, 0x5966ed86UL, 0x58a487b1UL, 0x5deb9134UL, 0x5c29fb03UL, 0x5e6f455aUL, 0x5fad2f6dUL, 0xe1351b80UL, 0xe0f771b7UL, 0xe2b1cfeeUL, 0xe373a5d9UL, 0xe63cb35cUL, 0xe7fed96bUL, 0xe5b86732UL, 0xe47a0d05UL, 0xef264a38UL, 0xeee4200fUL, 0xeca29e56UL, 0xed60f461UL,
      0xe82fe2e4UL, 0xe9ed88d3UL, 0xebab368aUL, 0xea695cbdUL, 0xfd13b8f0UL, 0xfcd1d2c7UL, 0xfe976c9eUL, 0xff5506a9UL, 0xfa1a102cUL, 0xfbd87a1bUL, 0xf99ec442UL, 0xf85cae75UL, 0xf300e948UL, 0xf2c2837fUL, 0xf0843d26UL, 0xf1465711UL, 0xf4094194UL, 0xf5cb2ba3UL, 0xf78d95faUL, 0xf64fffcdUL,
      0xd9785d60UL, 0xd8ba3757UL, 0xdafc890eUL, 0xdb3ee339UL, 0xde71f5bcUL, 0xdfb39f8bUL, 0xddf521d2UL, 0xdc374be5UL, 0xd76b0cd8UL, 0xd6a966efUL, 0xd4efd8b6UL, 0xd52db281UL, 0xd062a404UL, 0xd1a0ce33UL, 0xd3e6706aUL, 0xd2241a5dUL, 0xc55efe10UL, 0xc49c9427UL, 0xc6da2a7eUL, 0xc7184049UL,
      0xc25756ccUL, 0xc3953cfbUL, 0xc1d382a2UL, 0xc011e895UL, 0xcb4dafa8UL, 0xca8fc59fUL, 0xc8c97bc6UL, 0xc90b11f1UL, 0xcc440774UL, 0xcd866d43UL, 0xcfc0d31aUL, 0xce02b92dUL, 0x91af9640UL, 0x906dfc77UL, 0x922b422eUL, 0x93e92819UL, 0x96a63e9cUL, 0x976454abUL, 0x9522eaf2UL, 0x94e080c5UL,
      0x9fbcc7f8UL, 0x9e7eadcfUL, 0x9c381396UL, 0x9dfa79a1UL, 0x98b56f24UL, 0x99770513UL, 0x9b31bb4aUL, 0x9af3d17dUL, 0x8d893530UL, 0x8c4b5f07UL, 0x8e0de15eUL, 0x8fcf8b69UL, 0x8a809decUL, 0x8b42f7dbUL, 0x89044982UL, 0x88c623b5UL, 0x839a6488UL, 0x82580ebfUL, 0x801eb0e6UL, 0x81dcdad1UL,
      0x8493cc54UL, 0x8551a663UL, 0x8717183aUL, 0x86d5720dUL, 0xa9e2d0a0UL, 0xa820ba97UL, 0xaa6604ceUL, 0xaba46ef9UL, 0xaeeb787cUL, 0xaf29124bUL, 0xad6fac12UL, 0xacadc625UL, 0xa7f18118UL, 0xa633eb2fUL, 0xa4755576UL, 0xa5b73f41UL, 0xa0f829c4UL, 0xa13a43f3UL, 0xa37cfdaaUL, 0xa2be979dUL,
      0xb5c473d0UL, 0xb40619e7UL, 0xb640a7beUL, 0xb782cd89UL, 0xb2cddb0cUL, 0xb30fb13bUL, 0xb1490f62UL, 0xb08b6555UL, 0xbbd72268UL, 0xba15485fUL, 0xb853f606UL, 0xb9919c31UL, 0xbcde8ab4UL, 0xbd1ce083UL, 0xbf5a5edaUL, 0xbe9834edUL
     },
     {
      0x00000000UL, 0xb8bc6765UL, 0xaa09c88bUL, 0x12b5afeeUL, 0x8f629757UL, 0x37def032UL, 0x256b5fdcUL, 0x9dd738b9UL, 0xc5b428efUL, 0x7d084f8aUL, 0x6fbde064UL, 0xd7018701UL, 0x4ad6bfb8UL, 0xf26ad8ddUL, 0xe0df7733UL, 0x58631056UL, 0x5019579fUL, 0xe8a530faUL, 0xfa109f14UL, 0x42acf871UL,
      0xdf7bc0c8UL, 0x67c7a7adUL, 0x75720843UL, 0xcdce6f26UL, 0x95ad7f70UL, 0x2d111815UL, 0x3fa4b7fbUL, 0x8718d09eUL, 0x1acfe827UL, 0xa2738f42UL, 0xb0c620acUL, 0x087a47c9UL, 0xa032af3eUL, 0x188ec85bUL, 0x0a3b67b5UL, 0xb28700d0UL, 0x2f503869UL, 0x97ec5f0cUL, 0x8559f0e2UL, 0x3de59787UL,
      0x658687d1UL, 0xdd3ae0b4UL, 0xcf8f4f5aUL, 0x7733283fUL, 0xeae41086UL, 0x525877e3UL, 0x40edd80dUL, 0xf851bf68UL, 0xf02bf8a1UL, 0x48979fc4UL, 0x5a22302aUL, 0xe29e574fUL, 0x7f496ff6UL, 0xc7f50893UL, 0xd540a77dUL, 0x6dfcc018UL, 0x359fd04eUL, 0x8d23b72bUL, 0x9f9618c5UL, 0x272a7fa0UL,
      0xbafd4719UL, 0x0241207cUL, 0x10f48f92UL, 0xa848e8f7UL, 0x9b14583dUL, 0x23a83f58UL, 0x311d90b6UL, 0x89a1f7d3UL, 0x1476cf6aUL, 0xaccaa80fUL, 0xbe7f07e1UL, 0x06c36084UL, 0x5ea070d2UL, 0xe61c17b7UL, 0xf4a9b859UL, 0x4c15df3cUL, 0xd1c2e785UL, 0x697e80e0UL, 0x7bcb2f0eUL, 0xc377486bUL,
      0xcb0d0fa2UL, 0x73b168c7UL, 0x6104c729UL, 0xd9b8a04cUL, 0x446f98f5UL, 0xfcd3ff90UL, 0xee66507eUL, 0x56da371bUL, 0x0eb9274dUL, 0xb6054028UL, 0xa4b0efc6UL, 0x1c0c88a3UL, 0x81dbb01aUL, 0x3967d77fUL, 0x2bd27891UL, 0x936e1ff4UL, 0x3b26f703UL, 0x839a9066UL, 0x912f3f88UL, 0x299358edUL,
      0xb4446054UL, 0x0cf80731UL, 0x1e4da8dfUL, 0xa6f1cfbaUL, 0xfe92dfecUL, 0x462eb889UL, 0x549b1767UL, 0xec277002UL, 0x71f048bbUL, 0xc94c2fdeUL, 0xdbf98030UL, 0x6345e755UL, 0x6b3fa09cUL, 0xd383c7f9UL, 0xc1366817UL, 0x798a0f72UL, 0xe45d37cbUL, 0x5ce150aeUL, 0x4e54ff40UL, 0xf6e89825UL,
      0xae8b8873UL, 0x1637ef16UL, 0x048240f8UL, 0xbc3e279dUL, 0x21e91f24UL, 0x99557841UL, 0x8be0d7afUL, 0x335cb0caUL, 0xed59b63bUL, 0x55e5d15eUL, 0x47507eb0UL, 0xffec19d5UL, 0x623b216cUL, 0xda874609UL, 0xc832e9e7UL, 0x708e8e82UL, 0x28ed9ed4UL, 0x9051f9b1UL, 0x82e4565fUL, 0x3a58313aUL,
      0xa78f0983UL, 0x1f336ee6UL, 0x0d86c108UL, 0xb53aa66dUL, 0xbd40e1a4UL, 0x05fc86c1UL, 0x1749292fUL, 0xaff54e4aUL, 0x322276f3UL, 0x8a9e1196UL, 0x982bbe78UL, 0x2097d91dUL, 0x78f4c94bUL, 0xc048ae2eUL, 0xd2fd01c0UL, 0x6a4166a5UL, 0xf7965e1cUL, 0x4f2a3979UL, 0x5d9f9697UL, 0xe523f1f2UL,
      0x4d6b1905UL, 0xf5d77e60UL, 0xe762d18eUL, 0x5fdeb6ebUL, 0xc2098e52UL, 0x7ab5e937UL, 0x680046d9UL, 0xd0bc21bcUL, 0x88df31eaUL, 0x3063568fUL, 0x22d6f961UL, 0x9a6a9e04UL, 0x07bda6bdUL, 0xbf01c1d8UL, 0xadb46e36UL, 0x15080953UL, 0x1d724e9aUL, 0xa5ce29ffUL, 0xb77b8611UL, 0x0fc7e174UL,
      0x9210d9cdUL, 0x2aacbea8UL, 0x38191146UL, 0x80a57623UL, 0xd8c66675UL, 0x607a0110UL, 0x72cfaefeUL, 0xca73c99bUL, 0x57a4f122UL, 0xef189647UL, 0xfdad39a9UL, 0x45115eccUL, 0x764dee06UL, 0xcef18963UL, 0xdc44268dUL, 0x64f841e8UL, 0xf92f7951UL, 0x41931e34UL, 0x5326b1daUL, 0xeb9ad6bfUL,
      0xb3f9c6e9UL, 0x0b45a18cUL, 0x19f00e62UL, 0xa14c6907UL, 0x3c9b51beUL, 0x842736dbUL, 0x96929935UL, 0x2e2efe50UL, 0x2654b999UL, 0x9ee8defcUL, 0x8c5d7112UL, 0x34e11677UL, 0xa9362eceUL, 0x118a49abUL, 0x033fe645UL, 0xbb838120UL, 0xe3e09176UL, 0x5b5cf613UL, 0x49e959fdUL, 0xf1553e98UL,
      0x6c820621UL, 0xd43e6144UL, 0xc68bceaaUL, 0x7e37a9cfUL, 0xd67f4138UL, 0x6ec3265dUL, 0x7c7689b3UL, 0xc4caeed6UL, 0x591dd66fUL, 0xe1a1b10aUL, 0xf3141ee4UL, 0x4ba87981UL, 0x13cb69d7UL, 0xab770eb2UL, 0xb9c2a15cUL, 0x017ec639UL, 0x9ca9fe80UL, 0x241599e5UL, 0x36a0360bUL, 0x8e1c516eUL,
      0x866616a7UL, 0x3eda71c2UL, 0x2c6fde2cUL, 0x94d3b949UL, 0x090481f0UL, 0xb1b8e695UL, 0xa30d497bUL, 0x1bb12e1eUL, 0x43d23e48UL, 0xfb6e592dUL, 0xe9dbf6c3UL, 0x516791a6UL, 0xccb0a91fUL, 0x740cce7aUL, 0x66b96194UL, 0xde0506f1UL
     },
     {
      0x00000000UL, 0x96300777UL, 0x2c610eeeUL, 0xba510999UL, 0x19c46d07UL, 0x8ff46a70UL, 0x35a563e9UL, 0xa395649eUL, 0x3288db0eUL, 0xa4b8dc79UL, 0x1ee9d5e0UL, 0x88d9d297UL, 0x2b4cb609UL, 0xbd7cb17eUL, 0x072db8e7UL, 0x911dbf90UL, 0x6410b71dUL, 0xf220b06aUL, 0x4871b9f3UL, 0xde41be84UL,
      0x7dd4da1aUL, 0xebe4dd6dUL, 0x51b5d4f4UL, 0xc785d383UL, 0x56986c13UL, 0xc0a86b64UL, 0x7af962fdUL, 0xecc9658aUL, 0x4f5c0114UL, 0xd96c0663UL, 0x633d0ffaUL, 0xf50d088dUL, 0xc8206e3bUL, 0x5e10694cUL, 0xe44160d5UL, 0x727167a2UL, 0xd1e4033cUL, 0x47d4044bUL, 0xfd850dd2UL, 0x6bb50aa5UL,
      0xfaa8b535UL, 0x6c98b242UL, 0xd6c9bbdbUL, 0x40f9bcacUL, 0xe36cd832UL, 0x755cdf45UL, 0xcf0dd6dcUL, 0x593dd1abUL, 0xac30d926UL, 0x3a00de51UL, 0x8051d7c8UL, 0x1661d0bfUL, 0xb5f4b421UL, 0x23c4b356UL, 0x9995bacfUL, 0x0fa5bdb8UL, 0x9eb80228UL, 0x0888055fUL, 0xb2d90cc6UL, 0x24e90bb1UL,
      0x877c6f2fUL, 0x114c6858UL, 0xab1d61c1UL, 0x3d2d66b6UL, 0x9041dc76UL, 0x0671db01UL, 0xbc20d298UL, 0x2a10d5efUL, 0x8985b171UL, 0x1fb5b606UL, 0xa5e4bf9fUL, 0x33d4b8e8UL, 0xa2c90778UL, 0x34f9000fUL, 0x8ea80996UL, 0x18980ee1UL, 0xbb0d6a7fUL, 0x2d3d6d08UL, 0x976c6491UL, 0x015c63e6UL,
      0xf4516b6bUL, 0x62616c1cUL, 0xd8306585UL, 0x4e0062f2UL, 0xed95066cUL, 0x7ba5011bUL, 0xc1f40882UL, 0x57c40ff5UL, 0xc6d9b065UL, 0x50e9b712UL, 0xeab8be8bUL, 0x7c88b9fcUL, 0xdf1ddd62UL, 0x492dda15UL, 0xf37cd38cUL, 0x654cd4fbUL, 0x5861b24dUL, 0xce51b53aUL, 0x7400bca3UL, 0xe230bbd4UL,
      0x41a5df4aUL, 0xd795d83dUL, 0x6dc4d1a4UL, 0xfbf4d6d3UL, 0x6ae96943UL, 0xfcd96e34UL, 0x468867adUL, 0xd0b860daUL, 0x732d0444UL, 0xe51d0333UL, 0x5f4c0aaaUL, 0xc97c0dddUL, 0x3c710550UL, 0xaa410227UL, 0x10100bbeUL, 0x86200cc9UL, 0x25b56857UL, 0xb3856f20UL, 0x09d466b9UL, 0x9fe461ceUL,
      0x0ef9de5eUL, 0x98c9d929UL, 0x2298d0b0UL, 0xb4a8d7c7UL, 0x173db359UL, 0x810db42eUL, 0x3b5cbdb7UL, 0xad6cbac0UL, 0x2083b8edUL, 0xb6b3bf9aUL, 0x0ce2b603UL, 0x9ad2b174UL, 0x3947d5eaUL, 0xaf77d29dUL, 0x1526db04UL, 0x8316dc73UL, 0x120b63e3UL, 0x843b6494UL, 0x3e6a6d0dUL, 0xa85a6a7aUL,
      0x0bcf0ee4UL, 0x9dff0993UL, 0x27ae000aUL, 0xb19e077dUL, 0x44930ff0UL, 0xd2a30887UL, 0x68f2011eUL, 0xfec20669UL, 0x5d5762f7UL, 0xcb676580UL, 0x71366c19UL, 0xe7066b6eUL, 0x761bd4feUL, 0xe02bd389UL, 0x5a7ada10UL, 0xcc4add67UL, 0x6fdfb9f9UL, 0xf9efbe8eUL, 0x43beb717UL, 0xd58eb060UL,
      0xe8a3d6d6UL, 0x7e93d1a1UL, 0xc4c2d838UL, 0x52f2df4fUL, 0xf167bbd1UL, 0x6757bca6UL, 0xdd06b53fUL, 0x4b36b248UL, 0xda2b0dd8UL, 0x4c1b0aafUL, 0xf64a0336UL, 0x607a0441UL, 0xc3ef60dfUL, 0x55df67a8UL, 0xef8e6e31UL, 0x79be6946UL, 0x8cb361cbUL, 0x1a8366bcUL, 0xa0d26f25UL, 0x36e26852UL,
      0x95770cccUL, 0x03470bbbUL, 0xb9160222UL, 0x2f260555UL, 0xbe3bbac5UL, 0x280bbdb2UL, 0x925ab42bUL, 0x046ab35cUL, 0xa7ffd7c2UL, 0x31cfd0b5UL, 0x8b9ed92cUL, 0x1daede5bUL, 0xb0c2649bUL, 0x26f263ecUL, 0x9ca36a75UL, 0x0a936d02UL, 0xa906099cUL, 0x3f360eebUL, 0x85670772UL, 0x13570005UL,
      0x824abf95UL, 0x147ab8e2UL, 0xae2bb17bUL, 0x381bb60cUL, 0x9b8ed292UL, 0x0dbed5e5UL, 0xb7efdc7cUL, 0x21dfdb0bUL, 0xd4d2d386UL, 0x42e2d4f1UL, 0xf8b3dd68UL, 0x6e83da1fUL, 0xcd16be81UL, 0x5b26b9f6UL, 0xe177b06fUL, 0x7747b718UL, 0xe65a0888UL, 0x706a0fffUL, 0xca3b0666UL, 0x5c0b0111UL,
      0xff9e658fUL, 0x69ae62f8UL, 0xd3ff6b61UL, 0x45cf6c16UL, 0x78e20aa0UL, 0xeed20dd7UL, 0x5483044eUL, 0xc2b30339UL, 0x612667a7UL, 0xf71660d0UL, 0x4d476949UL, 0xdb776e3eUL, 0x4a6ad1aeUL, 0xdc5ad6d9UL, 0x660bdf40UL, 0xf03bd837UL, 0x53aebca9UL, 0xc59ebbdeUL, 0x7fcfb247UL, 0xe9ffb530UL,
      0x1cf2bdbdUL, 0x8ac2bacaUL, 0x3093b353UL, 0xa6a3b424UL, 0x0536d0baUL, 0x9306d7cdUL, 0x2957de54UL, 0xbf67d923UL, 0x2e7a66b3UL, 0xb84a61c4UL, 0x021b685dUL, 0x942b6f2aUL, 0x37be0bb4UL, 0xa18e0cc3UL, 0x1bdf055aUL, 0x8def022dUL
     },
     {
      0x00000000UL, 0x41311b19UL, 0x82623632UL, 0xc3532d2bUL, 0x04c56c64UL, 0x45f4777dUL, 0x86a75a56UL, 0xc796414fUL, 0x088ad9c8UL, 0x49bbc2d1UL, 0x8ae8effaUL, 0xcbd9f4e3UL, 0x0c4fb5acUL, 0x4d7eaeb5UL, 0x8e2d839eUL, 0xcf1c9887UL, 0x5112c24aUL, 0x1023d953UL, 0xd370f478UL, 0x9241ef61UL,
      0x55d7ae2eUL, 0x14e6b537UL, 0xd7b5981cUL, 0x96848305UL, 0x59981b82UL, 0x18a9009bUL, 0xdbfa2db0UL, 0x9acb36a9UL, 0x5d5d77e6UL, 0x1c6c6cffUL, 0xdf3f41d4UL, 0x9e0e5acdUL, 0xa2248495UL, 0xe3159f8cUL, 0x2046b2a7UL, 0x6177a9beUL, 0xa6e1e8f1UL, 0xe7d0f3e8UL, 0x2483dec3UL, 0x65b2c5daUL,
      0xaaae5d5dUL, 0xeb9f4644UL, 0x28cc6b6fUL, 0x69fd7076UL, 0xae6b3139UL, 0xef5a2a20UL, 0x2c09070bUL, 0x6d381c12UL, 0xf33646dfUL, 0xb2075dc6UL, 0x715470edUL, 0x30656bf4UL, 0xf7f32abbUL, 0xb6c231a2UL, 0x75911c89UL, 0x34a00790UL, 0xfbbc9f17UL, 0xba8d840eUL, 0x79dea925UL, 0x38efb23cUL,
      0xff79f373UL, 0xbe48e86aUL, 0x7d1bc541UL, 0x3c2ade58UL, 0x054f79f0UL, 0x447e62e9UL, 0x872d4fc2UL, 0xc61c54dbUL, 0x018a1594UL, 0x40bb0e8dUL, 0x83e823a6UL, 0xc2d938bfUL, 0x0dc5a038UL, 0x4cf4bb21UL, 0x8fa7960aUL, 0xce968d13UL, 0x0900cc5cUL, 0x4831d745UL, 0x8b62fa6eUL, 0xca53e177UL,
      0x545dbbbaUL, 0x156ca0a3UL, 0xd63f8d88UL, 0x970e9691UL, 0x5098d7deUL, 0x11a9ccc7UL, 0xd2fae1ecUL, 0x93cbfaf5UL, 0x5cd76272UL, 0x1de6796bUL, 0xdeb55440UL, 0x9f844f59UL, 0x58120e16UL, 0x1923150fUL, 0xda703824UL, 0x9b41233dUL, 0xa76bfd65UL, 0xe65ae67cUL, 0x2509cb57UL, 0x6438d04eUL,
      0xa3ae9101UL, 0xe29f8a18UL, 0x21cca733UL, 0x60fdbc2aUL, 0xafe124adUL, 0xeed03fb4UL, 0x2d83129fUL, 0x6cb20986UL, 0xab2448c9UL, 0xea1553d0UL, 0x29467efbUL, 0x687765e2UL, 0xf6793f2fUL, 0xb7482436UL, 0x741b091dUL, 0x352a1204UL, 0xf2bc534bUL, 0xb38d4852UL, 0x70de6579UL, 0x31ef7e60UL,
      0xfef3e6e7UL, 0xbfc2fdfeUL, 0x7c91d0d5UL, 0x3da0cbccUL, 0xfa368a83UL, 0xbb07919aUL, 0x7854bcb1UL, 0x3965a7a8UL, 0x4b98833bUL, 0x0aa99822UL, 0xc9fab509UL, 0x88cbae10UL, 0x4f5def5fUL, 0x0e6cf446UL, 0xcd3fd96dUL, 0x8c0ec274UL, 0x43125af3UL, 0x022341eaUL, 0xc1706cc1UL, 0x804177d8UL,
      0x47d73697UL, 0x06e62d8eUL, 0xc5b500a5UL, 0x84841bbcUL, 0x1a8a4171UL, 0x5bbb5a68UL, 0x98e87743UL, 0xd9d96c5aUL, 0x1e4f2d15UL, 0x5f7e360cUL, 0x9c2d1b27UL, 0xdd1c003eUL, 0x120098b9UL, 0x533183a0UL, 0x9062ae8bUL, 0xd153b592UL, 0x16c5f4ddUL, 0x57f4efc4UL, 0x94a7c2efUL, 0xd596d9f6UL,
      0xe9bc07aeUL, 0xa88d1cb7UL, 0x6bde319cUL, 0x2aef2a85UL, 0xed796bcaUL, 0xac4870d3UL, 0x6f1b5df8UL, 0x2e2a46e1UL, 0xe136de66UL, 0xa007c57fUL, 0x6354e854UL, 0x2265f34dUL, 0xe5f3b202UL, 0xa4c2a91bUL, 0x67918430UL, 0x26a09f29UL, 0xb8aec5e4UL, 0xf99fdefdUL, 0x3accf3d6UL, 0x7bfde8cfUL,
      0xbc6ba980UL, 0xfd5ab299UL, 0x3e099fb2UL, 0x7f3884abUL, 0xb0241c2cUL, 0xf1150735UL, 0x32462a1eUL, 0x73773107UL, 0xb4e17048UL, 0xf5d06b51UL, 0x3683467aUL, 0x77b25d63UL, 0x4ed7facbUL, 0x0fe6e1d2UL, 0xccb5ccf9UL, 0x8d84d7e0UL, 0x4a1296afUL, 0x0b238db6UL, 0xc870a09dUL, 0x8941bb84UL,
      0x465d2303UL, 0x076c381aUL, 0xc43f1531UL, 0x850e0e28UL, 0x42984f67UL, 0x03a9547eUL, 0xc0fa7955UL, 0x81cb624cUL, 0x1fc53881UL, 0x5ef42398UL, 0x9da70eb3UL, 0xdc9615aaUL, 0x1b0054e5UL, 0x5a314ffcUL, 0x996262d7UL, 0xd85379ceUL, 0x174fe149UL, 0x567efa50UL, 0x952dd77bUL, 0xd41ccc62UL,
      0x138a8d2dUL, 0x52bb9634UL, 0x91e8bb1fUL, 0xd0d9a006UL, 0xecf37e5eUL, 0xadc26547UL, 0x6e91486cUL, 0x2fa05375UL, 0xe836123aUL, 0xa9070923UL, 0x6a542408UL, 0x2b653f11UL, 0xe479a796UL, 0xa548bc8fUL, 0x661b91a4UL, 0x272a8abdUL, 0xe0bccbf2UL, 0xa18dd0ebUL, 0x62defdc0UL, 0x23efe6d9UL,
      0xbde1bc14UL, 0xfcd0a70dUL, 0x3f838a26UL, 0x7eb2913fUL, 0xb924d070UL, 0xf815cb69UL, 0x3b46e642UL, 0x7a77fd5bUL, 0xb56b65dcUL, 0xf45a7ec5UL, 0x370953eeUL, 0x763848f7UL, 0xb1ae09b8UL, 0xf09f12a1UL, 0x33cc3f8aUL, 0x72fd2493UL
     },
     {
      0x00000000UL, 0x376ac201UL, 0x6ed48403UL, 0x59be4602UL, 0xdca80907UL, 0xebc2cb06UL, 0xb27c8d04UL, 0x85164f05UL, 0xb851130eUL, 0x8f3bd10fUL, 0xd685970dUL, 0xe1ef550cUL, 0x64f91a09UL, 0x5393d808UL, 0x0a2d9e0aUL, 0x3d475c0bUL, 0x70a3261cUL, 0x47c9e41dUL, 0x1e77a21fUL, 0x291d601eUL,
      0xac0b2f1bUL, 0x9b61ed1aUL, 0xc2dfab18UL, 0xf5b56919UL, 0xc8f23512UL, 0xff98f713UL, 0xa626b111UL, 0x914c7310UL, 0x145a3c15UL, 0x2330fe14UL, 0x7a8eb816UL, 0x4de47a17UL, 0xe0464d38UL, 0xd72c8f39UL, 0x8e92c93bUL, 0xb9f80b3aUL, 0x3cee443fUL, 0x0b84863eUL, 0x523ac03cUL, 0x6550023dUL,
      0x58175e36UL, 0x6f7d9c37UL, 0x36c3da35UL, 0x01a91834UL, 0x84bf5731UL, 0xb3d59530UL, 0xea6bd332UL, 0xdd011133UL, 0x90e56b24UL, 0xa78fa925UL, 0xfe31ef27UL, 0xc95b2d26UL, 0x4c4d6223UL, 0x7b27a022UL, 0x2299e620UL, 0x15f32421UL, 0x28b4782aUL, 0x1fdeba2bUL, 0x4660fc29UL, 0x710a3e28UL,
      0xf41c712dUL, 0xc376b32cUL, 0x9ac8f52eUL, 0xada2372fUL, 0xc08d9a70UL, 0xf7e75871UL, 0xae591e73UL, 0x9933dc72UL, 0x1c259377UL, 0x2b4f5176UL, 0x72f11774UL, 0x459bd575UL, 0x78dc897eUL, 0x4fb64b7fUL, 0x16080d7dUL, 0x2162cf7cUL, 0xa4748079UL, 0x931e4278UL, 0xcaa0047aUL, 0xfdcac67bUL,
      0xb02ebc6cUL, 0x87447e6dUL, 0xdefa386fUL, 0xe990fa6eUL, 0x6c86b56bUL, 0x5bec776aUL, 0x02523168UL, 0x3538f369UL, 0x087faf62UL, 0x3f156d63UL, 0x66ab2b61UL, 0x51c1e960UL, 0xd4d7a665UL, 0xe3bd6464UL, 0xba032266UL, 0x8d69e067UL, 0x20cbd748UL, 0x17a11549UL, 0x4e1f534bUL, 0x7975914aUL,
      0xfc63de4fUL, 0xcb091c4eUL, 0x92b75a4cUL, 0xa5dd984dUL, 0x989ac446UL, 0xaff00647UL, 0xf64e4045UL, 0xc1248244UL, 0x4432cd41UL, 0x73580f40UL, 0x2ae64942UL, 0x1d8c8b43UL, 0x5068f154UL, 0x67023355UL, 0x3ebc7557UL, 0x09d6b756UL, 0x8cc0f853UL, 0xbbaa3a52UL, 0xe2147c50UL, 0xd57ebe51UL,
      0xe839e25aUL, 0xdf53205bUL, 0x86ed6659UL, 0xb187a458UL, 0x3491eb5dUL, 0x03fb295cUL, 0x5a456f5eUL, 0x6d2fad5fUL, 0x801b35e1UL, 0xb771f7e0UL, 0xeecfb1e2UL, 0xd9a573e3UL, 0x5cb33ce6UL, 0x6bd9fee7UL, 0x3267b8e5UL, 0x050d7ae4UL, 0x384a26efUL, 0x0f20e4eeUL, 0x569ea2ecUL, 0x61f460edUL,
      0xe4e22fe8UL, 0xd388ede9UL, 0x8a36abebUL, 0xbd5c69eaUL, 0xf0b813fdUL, 0xc7d2d1fcUL, 0x9e6c97feUL, 0xa90655ffUL, 0x2c101afaUL, 0x1b7ad8fbUL, 0x42c49ef9UL, 0x75ae5cf8UL, 0x48e900f3UL, 0x7f83c2f2UL, 0x263d84f0UL, 0x115746f1UL, 0x944109f4UL, 0xa32bcbf5UL, 0xfa958df7UL, 0xcdff4ff6UL,
      0x605d78d9UL, 0x5737bad8UL, 0x0e89fcdaUL, 0x39e33edbUL, 0xbcf571deUL, 0x8b9fb3dfUL, 0xd221f5ddUL, 0xe54b37dcUL, 0xd80c6bd7UL, 0xef66a9d6UL, 0xb6d8efd4UL, 0x81b22dd5UL, 0x04a462d0UL, 0x33cea0d1UL, 0x6a70e6d3UL, 0x5d1a24d2UL, 0x10fe5ec5UL, 0x27949cc4UL, 0x7e2adac6UL, 0x494018c7UL,
      0xcc5657c2UL, 0xfb3c95c3UL, 0xa282d3c1UL, 0x95e811c0UL, 0xa8af4dcbUL, 0x9fc58fcaUL, 0xc67bc9c8UL, 0xf1110bc9UL, 0x740744ccUL, 0x436d86cdUL, 0x1ad3c0cfUL, 0x2db902ceUL, 0x4096af91UL, 0x77fc6d90UL, 0x2e422b92UL, 0x1928e993UL, 0x9c3ea696UL, 0xab546497UL, 0xf2ea2295UL, 0xc580e094UL,
      0xf8c7bc9fUL, 0xcfad7e9eUL, 0x9613389cUL, 0xa179fa9dUL, 0x246fb598UL, 0x13057799UL, 0x4abb319bUL, 0x7dd1f39aUL, 0x3035898dUL, 0x075f4b8cUL, 0x5ee10d8eUL, 0x698bcf8fUL, 0xec9d808aUL, 0xdbf7428bUL, 0x82490489UL, 0xb523c688UL, 0x88649a83UL, 0xbf0e5882UL, 0xe6b01e80UL, 0xd1dadc81UL,
      0x54cc9384UL, 0x63a65185UL, 0x3a181787UL, 0x0d72d586UL, 0xa0d0e2a9UL, 0x97ba20a8UL, 0xce0466aaUL, 0xf96ea4abUL, 0x7c78ebaeUL, 0x4b1229afUL, 0x12ac6fadUL, 0x25c6adacUL, 0x1881f1a7UL, 0x2feb33a6UL, 0x765575a4UL, 0x413fb7a5UL, 0xc429f8a0UL, 0xf3433aa1UL, 0xaafd7ca3UL, 0x9d97bea2UL,
      0xd073c4b5UL, 0xe71906b4UL, 0xbea740b6UL, 0x89cd82b7UL, 0x0cdbcdb2UL, 0x3bb10fb3UL, 0x620f49b1UL, 0x55658bb0UL, 0x6822d7bbUL, 0x5f4815baUL,0x06f653b8UL, 0x319c91b9UL, 0xb48adebcUL, 0x83e01cbdUL, 0xda5e5abfUL, 0xed3498beUL
     },
     {
      0x00000000UL, 0x6567bcb8UL, 0x8bc809aaUL, 0xeeafb512UL, 0x5797628fUL, 0x32f0de37UL, 0xdc5f6b25UL, 0xb938d79dUL, 0xef28b4c5UL, 0x8a4f087dUL,
      0x64e0bd6fUL, 0x018701d7UL, 0xb8bfd64aUL, 0xddd86af2UL, 0x3377dfe0UL, 0x56106358UL, 0x9f571950UL, 0xfa30a5e8UL, 0x149f10faUL, 0x71f8ac42UL,
      0xc8c07bdfUL, 0xada7c767UL, 0x43087275UL, 0x266fcecdUL, 0x707fad95UL, 0x1518112dUL, 0xfbb7a43fUL, 0x9ed01887UL, 0x27e8cf1aUL, 0x428f73a2UL,
      0xac20c6b0UL, 0xc9477a08UL, 0x3eaf32a0UL, 0x5bc88e18UL, 0xb5673b0aUL, 0xd00087b2UL, 0x6938502fUL, 0x0c5fec97UL, 0xe2f05985UL, 0x8797e53dUL,
      0xd1878665UL, 0xb4e03addUL, 0x5a4f8fcfUL, 0x3f283377UL, 0x8610e4eaUL, 0xe3775852UL, 0x0dd8ed40UL, 0x68bf51f8UL, 0xa1f82bf0UL, 0xc49f9748UL,
      0x2a30225aUL, 0x4f579ee2UL, 0xf66f497fUL, 0x9308f5c7UL, 0x7da740d5UL, 0x18c0fc6dUL, 0x4ed09f35UL, 0x2bb7238dUL, 0xc518969fUL, 0xa07f2a27UL,
      0x1947fdbaUL, 0x7c204102UL, 0x928ff410UL, 0xf7e848a8UL, 0x3d58149bUL, 0x583fa823UL, 0xb6901d31UL, 0xd3f7a189UL, 0x6acf7614UL, 0x0fa8caacUL,
      0xe1077fbeUL, 0x8460c306UL, 0xd270a05eUL, 0xb7171ce6UL, 0x59b8a9f4UL, 0x3cdf154cUL, 0x85e7c2d1UL, 0xe0807e69UL, 0x0e2fcb7bUL, 0x6b4877c3UL,
      0xa20f0dcbUL, 0xc768b173UL, 0x29c70461UL, 0x4ca0b8d9UL, 0xf5986f44UL, 0x90ffd3fcUL, 0x7e5066eeUL, 0x1b37da56UL, 0x4d27b90eUL, 0x284005b6UL,
      0xc6efb0a4UL, 0xa3880c1cUL, 0x1ab0db81UL, 0x7fd76739UL, 0x9178d22bUL, 0xf41f6e93UL, 0x03f7263bUL, 0x66909a83UL, 0x883f2f91UL, 0xed589329UL,
      0x546044b4UL, 0x3107f80cUL, 0xdfa84d1eUL, 0xbacff1a6UL, 0xecdf92feUL, 0x89b82e46UL, 0x67179b54UL, 0x027027ecUL, 0xbb48f071UL, 0xde2f4cc9UL,
      0x3080f9dbUL, 0x55e74563UL, 0x9ca03f6bUL, 0xf9c783d3UL, 0x176836c1UL, 0x720f8a79UL, 0xcb375de4UL, 0xae50e15cUL, 0x40ff544eUL, 0x2598e8f6UL,
      0x73888baeUL, 0x16ef3716UL, 0xf8408204UL, 0x9d273ebcUL, 0x241fe921UL, 0x41785599UL, 0xafd7e08bUL, 0xcab05c33UL, 0x3bb659edUL, 0x5ed1e555UL,
      0xb07e5047UL, 0xd519ecffUL, 0x6c213b62UL, 0x094687daUL, 0xe7e932c8UL, 0x828e8e70UL, 0xd49eed28UL, 0xb1f95190UL, 0x5f56e482UL, 0x3a31583aUL,
      0x83098fa7UL, 0xe66e331fUL, 0x08c1860dUL, 0x6da63ab5UL, 0xa4e140bdUL, 0xc186fc05UL, 0x2f294917UL, 0x4a4ef5afUL, 0xf3762232UL, 0x96119e8aUL,
      0x78be2b98UL, 0x1dd99720UL, 0x4bc9f478UL, 0x2eae48c0UL, 0xc001fdd2UL, 0xa566416aUL, 0x1c5e96f7UL, 0x79392a4fUL, 0x97969f5dUL, 0xf2f123e5UL,
      0x05196b4dUL, 0x607ed7f5UL, 0x8ed162e7UL, 0xebb6de5fUL, 0x528e09c2UL, 0x37e9b57aUL, 0xd9460068UL, 0xbc21bcd0UL, 0xea31df88UL, 0x8f566330UL,
      0x61f9d622UL, 0x049e6a9aUL, 0xbda6bd07UL, 0xd8c101bfUL, 0x366eb4adUL, 0x53090815UL, 0x9a4e721dUL, 0xff29cea5UL, 0x11867bb7UL, 0x74e1c70fUL,
      0xcdd91092UL, 0xa8beac2aUL, 0x46111938UL, 0x2376a580UL, 0x7566c6d8UL, 0x10017a60UL, 0xfeaecf72UL, 0x9bc973caUL, 0x22f1a457UL, 0x479618efUL,
      0xa939adfdUL, 0xcc5e1145UL, 0x06ee4d76UL, 0x6389f1ceUL, 0x8d2644dcUL, 0xe841f864UL, 0x51792ff9UL, 0x341e9341UL, 0xdab12653UL, 0xbfd69aebUL,
      0xe9c6f9b3UL, 0x8ca1450bUL, 0x620ef019UL, 0x07694ca1UL, 0xbe519b3cUL, 0xdb362784UL, 0x35999296UL, 0x50fe2e2eUL, 0x99b95426UL, 0xfcdee89eUL,
      0x12715d8cUL, 0x7716e134UL, 0xce2e36a9UL, 0xab498a11UL, 0x45e63f03UL, 0x208183bbUL, 0x7691e0e3UL, 0x13f65c5bUL, 0xfd59e949UL, 0x983e55f1UL,
      0x2106826cUL, 0x44613ed4UL, 0xaace8bc6UL, 0xcfa9377eUL, 0x38417fd6UL, 0x5d26c36eUL, 0xb389767cUL, 0xd6eecac4UL, 0x6fd61d59UL, 0x0ab1a1e1UL,
      0xe41e14f3UL, 0x8179a84bUL, 0xd769cb13UL, 0xb20e77abUL, 0x5ca1c2b9UL, 0x39c67e01UL, 0x80fea99cUL, 0xe5991524UL, 0x0b36a036UL, 0x6e511c8eUL,
      0xa7166686UL, 0xc271da3eUL, 0x2cde6f2cUL, 0x49b9d394UL, 0xf0810409UL, 0x95e6b8b1UL, 0x7b490da3UL, 0x1e2eb11bUL, 0x483ed243UL, 0x2d596efbUL,
      0xc3f6dbe9UL, 0xa6916751UL, 0x1fa9b0ccUL, 0x7ace0c74UL, 0x9461b966UL, 0xf10605deUL
     }
  };
//---
const int32_t  SZlib::base_length[29]={ 0, 1, 2, 3, 4, 5, 6, 7, 8, 10, 12, 14, 16, 20, 24, 28, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 0 };
const int32_t  SZlib::base_dist[30]  ={ 0, 1, 2, 3, 4, 6, 8, 12, 16, 24, 32, 48, 64, 96,128, 192, 256, 384, 512,  768, 1024, 1536, 2048, 3072, 4096, 6144, 8192, 12288, 16384, 24576 };
const BYTE SZlib::bl_order[19]   ={ 16,17,18,0,8,7,9,6,10,5,11,4,12,3,13,2,14,1,15 };
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
const BYTE SZlib::length_code[256]=
  {
    0,  1,  2,  3,  4,  5,  6,  7,  8,  8,  9,  9, 10, 10, 11, 11, 12, 12, 12, 12,
   13, 13, 13, 13, 14, 14, 14, 14, 15, 15, 15, 15, 16, 16, 16, 16, 16, 16, 16, 16,
   17, 17, 17, 17, 17, 17, 17, 17, 18, 18, 18, 18, 18, 18, 18, 18, 19, 19, 19, 19,
   19, 19, 19, 19, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20, 20,
   21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 21, 22, 22, 22, 22,
   22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 22, 23, 23, 23, 23, 23, 23, 23, 23,
   23, 23, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
   24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
   25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
   25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 26, 26, 26, 26, 26, 26, 26, 26,
   26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
   26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
   27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 28
   };
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
const BYTE SZlib::dist_code[512]=
  {
    0,  1,  2,  3,  4,  4,  5,  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  8,
    8,  8,  8,  8,  9,  9,  9,  9,  9,  9,  9,  9, 10, 10, 10, 10, 10, 10, 10, 10,
   10, 10, 10, 10, 10, 10, 10, 10, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11, 11,
   11, 11, 11, 11, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12,
   12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 12, 13, 13, 13, 13,
   13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
   13, 13, 13, 13, 13, 13, 13, 13, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
   14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
   14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14,
   14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 14, 15, 15, 15, 15, 15, 15, 15, 15,
   15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
   15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,
   15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15, 15,  0,  0, 16, 17,
   18, 18, 19, 19, 20, 20, 20, 20, 21, 21, 21, 21, 22, 22, 22, 22, 22, 22, 22, 22,
   23, 23, 23, 23, 23, 23, 23, 23, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
   24, 24, 24, 24, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25, 25,
   26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26,
   26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 26, 27, 27, 27, 27, 27, 27, 27, 27,
   27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27, 27,
   27, 27, 27, 27, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
   28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
   28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28, 28,
   28, 28, 28, 28, 28, 28, 28, 28, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
   29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
   29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29,
   29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29, 29
   };
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
const SZlib::ct_data SZlib::static_ltree[288]=
  {
     {{ 12},{  8}}, {{140},{  8}}, {{ 76},{  8}}, {{204},{  8}}, {{ 44},{  8}}, {{172},{  8}}, {{108},{  8}}, {{236},{  8}}, {{ 28},{  8}}, {{156},{  8}},
   {{ 92},{  8}}, {{220},{  8}}, {{ 60},{  8}}, {{188},{  8}}, {{124},{  8}}, {{252},{  8}}, {{  2},{  8}}, {{130},{  8}}, {{ 66},{  8}}, {{194},{  8}},
   {{ 34},{  8}}, {{162},{  8}}, {{ 98},{  8}}, {{226},{  8}}, {{ 18},{  8}}, {{146},{  8}}, {{ 82},{  8}}, {{210},{  8}}, {{ 50},{  8}}, {{178},{  8}},
   {{114},{  8}}, {{242},{  8}}, {{ 10},{  8}}, {{138},{  8}}, {{ 74},{  8}}, {{202},{  8}}, {{ 42},{  8}}, {{170},{  8}}, {{106},{  8}}, {{234},{  8}},
   {{ 26},{  8}}, {{154},{  8}}, {{ 90},{  8}}, {{218},{  8}}, {{ 58},{  8}}, {{186},{  8}}, {{122},{  8}}, {{250},{  8}}, {{  6},{  8}}, {{134},{  8}},
   {{ 70},{  8}}, {{198},{  8}}, {{ 38},{  8}}, {{166},{  8}}, {{102},{  8}}, {{230},{  8}}, {{ 22},{  8}}, {{150},{  8}}, {{ 86},{  8}}, {{214},{  8}},
   {{ 54},{  8}}, {{182},{  8}}, {{118},{  8}}, {{246},{  8}}, {{ 14},{  8}}, {{142},{  8}}, {{ 78},{  8}}, {{206},{  8}}, {{ 46},{  8}}, {{174},{  8}},
   {{110},{  8}}, {{238},{  8}}, {{ 30},{  8}}, {{158},{  8}}, {{ 94},{  8}}, {{222},{  8}}, {{ 62},{  8}}, {{190},{  8}}, {{126},{  8}}, {{254},{  8}},
   {{  1},{  8}}, {{129},{  8}}, {{ 65},{  8}}, {{193},{  8}}, {{ 33},{  8}}, {{161},{  8}}, {{ 97},{  8}}, {{225},{  8}}, {{ 17},{  8}}, {{145},{  8}},
   {{ 81},{  8}}, {{209},{  8}}, {{ 49},{  8}}, {{177},{  8}}, {{113},{  8}}, {{241},{  8}}, {{  9},{  8}}, {{137},{  8}}, {{ 73},{  8}}, {{201},{  8}},
   {{ 41},{  8}}, {{169},{  8}}, {{105},{  8}}, {{233},{  8}}, {{ 25},{  8}}, {{153},{  8}}, {{ 89},{  8}}, {{217},{  8}}, {{ 57},{  8}}, {{185},{  8}},
   {{121},{  8}}, {{249},{  8}}, {{  5},{  8}}, {{133},{  8}}, {{ 69},{  8}}, {{197},{  8}}, {{ 37},{  8}}, {{165},{  8}}, {{101},{  8}}, {{229},{  8}},
   {{ 21},{  8}}, {{149},{  8}}, {{ 85},{  8}}, {{213},{  8}}, {{ 53},{  8}}, {{181},{  8}}, {{117},{  8}}, {{245},{  8}}, {{ 13},{  8}}, {{141},{  8}},
   {{ 77},{  8}}, {{205},{  8}}, {{ 45},{  8}}, {{173},{  8}}, {{109},{  8}}, {{237},{  8}}, {{ 29},{  8}}, {{157},{  8}}, {{ 93},{  8}}, {{221},{  8}},
   {{ 61},{  8}}, {{189},{  8}}, {{125},{  8}}, {{253},{  8}}, {{ 19},{  9}}, {{275},{  9}}, {{147},{  9}}, {{403},{  9}}, {{ 83},{  9}}, {{339},{  9}},
   {{211},{  9}}, {{467},{  9}}, {{ 51},{  9}}, {{307},{  9}}, {{179},{  9}}, {{435},{  9}}, {{115},{  9}}, {{371},{  9}}, {{243},{  9}}, {{499},{  9}},
   {{ 11},{  9}}, {{267},{  9}}, {{139},{  9}}, {{395},{  9}}, {{ 75},{  9}}, {{331},{  9}}, {{203},{  9}}, {{459},{  9}}, {{ 43},{  9}}, {{299},{  9}},
   {{171},{  9}}, {{427},{  9}}, {{107},{  9}}, {{363},{  9}}, {{235},{  9}}, {{491},{  9}}, {{ 27},{  9}}, {{283},{  9}}, {{155},{  9}}, {{411},{  9}},
   {{ 91},{  9}}, {{347},{  9}}, {{219},{  9}}, {{475},{  9}}, {{ 59},{  9}}, {{315},{  9}}, {{187},{  9}}, {{443},{  9}}, {{123},{  9}}, {{379},{  9}},
   {{251},{  9}}, {{507},{  9}}, {{  7},{  9}}, {{263},{  9}}, {{135},{  9}}, {{391},{  9}}, {{ 71},{  9}}, {{327},{  9}}, {{199},{  9}}, {{455},{  9}},
   {{ 39},{  9}}, {{295},{  9}}, {{167},{  9}}, {{423},{  9}}, {{103},{  9}}, {{359},{  9}}, {{231},{  9}}, {{487},{  9}}, {{ 23},{  9}}, {{279},{  9}},
   {{151},{  9}}, {{407},{  9}}, {{ 87},{  9}}, {{343},{  9}}, {{215},{  9}}, {{471},{  9}}, {{ 55},{  9}}, {{311},{  9}}, {{183},{  9}}, {{439},{  9}},
   {{119},{  9}}, {{375},{  9}}, {{247},{  9}}, {{503},{  9}}, {{ 15},{  9}}, {{271},{  9}}, {{143},{  9}}, {{399},{  9}}, {{ 79},{  9}}, {{335},{  9}},
   {{207},{  9}}, {{463},{  9}}, {{ 47},{  9}}, {{303},{  9}}, {{175},{  9}}, {{431},{  9}}, {{111},{  9}}, {{367},{  9}}, {{239},{  9}}, {{495},{  9}},
   {{ 31},{  9}}, {{287},{  9}}, {{159},{  9}}, {{415},{  9}}, {{ 95},{  9}}, {{351},{  9}}, {{223},{  9}}, {{479},{  9}}, {{ 63},{  9}}, {{319},{  9}},
   {{191},{  9}}, {{447},{  9}}, {{127},{  9}}, {{383},{  9}}, {{255},{  9}}, {{511},{  9}}, {{  0},{  7}}, {{ 64},{  7}}, {{ 32},{  7}}, {{ 96},{  7}},
   {{ 16},{  7}}, {{ 80},{  7}}, {{ 48},{  7}}, {{112},{  7}}, {{  8},{  7}}, {{ 72},{  7}}, {{ 40},{  7}}, {{104},{  7}}, {{ 24},{  7}}, {{ 88},{  7}},
   {{ 56},{  7}}, {{120},{  7}}, {{  4},{  7}}, {{ 68},{  7}}, {{ 36},{  7}}, {{100},{  7}}, {{ 20},{  7}}, {{ 84},{  7}}, {{ 52},{  7}}, {{116},{  7}},
   {{  3},{  8}}, {{131},{  8}}, {{ 67},{  8}}, {{195},{  8}}, {{ 35},{  8}}, {{163},{  8}}, {{ 99},{  8}}, {{227},{  8}}
  };
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
const SZlib::ct_data SZlib::static_dtree[30]=
  {
     {{ 0},{ 5}}, {{16},{ 5}}, {{ 8},{ 5}}, {{24},{ 5}}, {{ 4},{ 5}},
   {{20},{ 5}}, {{12},{ 5}}, {{28},{ 5}}, {{ 2},{ 5}}, {{18},{ 5}},
   {{10},{ 5}}, {{26},{ 5}}, {{ 6},{ 5}}, {{22},{ 5}}, {{14},{ 5}},
   {{30},{ 5}}, {{ 1},{ 5}}, {{17},{ 5}}, {{ 9},{ 5}}, {{25},{ 5}},
   {{ 5},{ 5}}, {{21},{ 5}}, {{13},{ 5}}, {{29},{ 5}}, {{ 3},{ 5}},
   {{19},{ 5}}, {{11},{ 5}}, {{27},{ 5}}, {{ 7},{ 5}}, {{23},{ 5}}
  };
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
const int32_t SZlib::extra_lbits[29] ={ 0,0,0,0,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3,3,4,4,4,4,5,5,5,5,0 };
const int32_t SZlib::extra_dbits[30] ={ 0,0,0,0,1,1,2,2,3,3,4,4,5,5,6,6,7,7,8,8,9,9,10,10,11,11,12,12,13,13 };
const int32_t SZlib::extra_blbits[19]={ 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,2,3,7 };

const SZlib::static_tree_desc SZlib::static_l_desc ={SZlib::static_ltree, SZlib::extra_lbits, 257, 286, 15};
const SZlib::static_tree_desc SZlib::static_d_desc ={SZlib::static_dtree, SZlib::extra_dbits, 0, 30, 15};
const SZlib::static_tree_desc SZlib::static_bl_desc={(const ct_data *)0, SZlib::extra_blbits, 0, 19, 7};
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
void* SZlib::zcalloc(unsigned items,unsigned size)
  {
   return malloc(items*size);
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
void SZlib::zcfree(void *ptr)
  {
   free(ptr);
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
int SZlib::read_buf(z_stream *strm,char *buf,unsigned size)
  {
   unsigned len=strm->avail_in;
//---
   if(len>size) len=size;
   if(len==0)
      return(0);
//---
   strm->avail_in-=len;

   if(strm->state->wrap==1)
      strm->adler=adler32(strm->adler,(BYTE*)strm->next_in,len);
   else
      if(strm->state->wrap==2)
         strm->adler=crc32(strm->adler,(BYTE*)strm->next_in,len);
//---
   memcpy(buf,strm->next_in,len);
   strm->next_in +=len;
   strm->total_in+=len;
//---
   return(int)len;
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
void SZlib::fill_window(internal_state *s)
  {
   unsigned   n,m;
   WORD      *p;
   unsigned   more;
   uint32_t   wsize=s->w_size;
   do
     {
      more=(unsigned)((s->window_size-(uint32_t)s->lookahead)-(uint32_t)s->strstart);
      //---
      if(s->strstart>=wsize+(s->w_size-262))
        {
         memcpy(s->window,s->window+wsize,(unsigned)wsize);
         s->match_start-=wsize;
         s->strstart   -=wsize;
         s->block_start-=(long) wsize;
         n=s->hash_size;
         p=&s->head[n];
         //---
         do
           {
            m=*--p;
            *p=(WORD)(m>=wsize ? m-wsize : 0);
           }
         while(--n);
         //---
         n=wsize;
         p=&s->prev[n];
         //---
         do
           {
            m=*--p;
            *p=(WORD)(m>=wsize ? m-wsize : 0);
           }
         while(--n);
         //---
         more+=wsize;
        }
      if(s->strm->avail_in==0) return;
      //---
      n=(uint32_t)read_buf(s->strm,s->window+s->strstart+s->lookahead,more);
      s->lookahead+=n;
      if(s->lookahead>=3)
        {
         s->ins_h=uint32_t(s->window[s->strstart]);
         s->ins_h=uint32_t((((s->ins_h)<<s->hash_shift)^(s->window[s->strstart+1]))&s->hash_mask);
        }
     } while(s->lookahead<262 && s->strm->avail_in!=0);
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
uint32_t SZlib::longest_match(internal_state *s,unsigned cur_match)
  {
   unsigned   chain_length=s->max_chain_length;
   char      *scan        =s->window+s->strstart;
   char      *match;
   int32_t        len;
   int32_t        best_len    =(int)s->prev_length;
   int32_t        nice_match  =s->nice_match;
   unsigned   limit       =s->strstart>(unsigned)(s->w_size-262) ? s->strstart -(unsigned)(s->w_size-262) : 0;
   WORD      *prev        =s->prev;
   uint32_t   wmask       =s->w_mask;
   char      *strend      =s->window+s->strstart+258;
   char       scan_end1   =scan[best_len-1];
   char       scan_end    =scan[best_len];
//---
   if(s->prev_length>=s->good_match)
      chain_length>>=2;
//---
   if(nice_match>(int)s->lookahead)
      nice_match=(int)s->lookahead;
//---
   do
     {
      //---
      match=s->window+cur_match;
      //---
      if(match[best_len]!=scan_end || match[best_len-1]!=scan_end1 || *match!=*scan || *++match!=scan[1])
         continue;
      //---
      scan+=2, match++;
      //---
      do
        {
        }
      while(*++scan==*++match && *++scan==*++match &&
            *++scan==*++match && *++scan==*++match &&
            *++scan==*++match && *++scan==*++match &&
            *++scan==*++match && *++scan==*++match && scan<strend);
      //---
      len=258-(int)(strend-scan);
      scan=strend-258;
      //---
      if(len>best_len)
        {
         s->match_start=cur_match;
         best_len=len;
         //---
         if(len>=nice_match) break;
         //---
         scan_end1=scan[best_len-1];
         scan_end =scan[best_len];
        }
     }
   while((cur_match=prev[cur_match & wmask])>limit && --chain_length!=0);
//---
   if((uint32_t)best_len<=s->lookahead) return((uint32_t)best_len);
//---
   return(s->lookahead);
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
uint32_t SZlib::longest_match_fast(internal_state *s,unsigned cur_match)
  {
   char *scan=s->window+s->strstart;
   char *match;
   int32_t   len;
   char *strend=s->window+s->strstart+258;
//---
   match=s->window + cur_match;
//---
   if(match[0]!=scan[0] || match[1]!=scan[1])
      return(2);
//---
   scan+=2,match+=2;
//---
   do
     {
     } while(*++scan==*++match && *++scan==*++match &&
*++scan==*++match && *++scan==*++match &&
*++scan==*++match && *++scan==*++match &&
*++scan==*++match && *++scan==*++match && scan<strend);
//---
   len=258-(int)(strend-scan);
//---
   if(len<3)
      return(2);
   s->match_start=cur_match;
//---
   return(uint32_t)len<=s->lookahead ? (uint32_t)len : s->lookahead;
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
SZlib::block_state SZlib::deflate_stored(internal_state *s,int flush)
  {
   uint32_t max_block_size=0xffff;
   uint32_t max_start;
//---
   if(max_block_size>s->pending_buf_size-5)
      max_block_size=s->pending_buf_size-5;
//---
   for(;;)
     {
      //---
      if(s->lookahead<=1)
        {
         fill_window(s);
         if(s->lookahead==0 && flush==0)
            return(NEED_MORE);
         if(s->lookahead==0)
            break;
        }
      //---
      s->strstart +=s->lookahead;
      s->lookahead =0;
      max_start    =uint32_t(s->block_start+max_block_size);
      //---
      if(s->strstart==0 || (uint32_t)s->strstart>=max_start)
        {
         //---
         s->lookahead=(uint32_t)(s->strstart-max_start);
         s->strstart =(uint32_t)max_start;
         //---
         _tr_flush_block(s,s->block_start>=0L?(char *)&s->window[(unsigned)s->block_start] : NULL,(uint32_t)((long)s->strstart-s->block_start),0);
         s->block_start=(long)s->strstart;
         flush_pending(s->strm);
         if(s->strm->avail_out==0) return(NEED_MORE);
        }
      //---
      if(s->strstart-(uint32_t)s->block_start>=((s)->w_size-(258+3+1)))
        {
         _tr_flush_block(s,s->block_start>=0L ? (char *)&s->window[(unsigned)s->block_start] : NULL,(uint32_t)((long)s->strstart - s->block_start),0);
         s->block_start=(long)s->strstart;
         flush_pending(s->strm);
         if(s->strm->avail_out==0) return(NEED_MORE);
        }
     }
//---
   _tr_flush_block(s,s->block_start>=0L ? (char *)&s->window[(unsigned)s->block_start] : NULL,(uint32_t)((long)s->strstart - s->block_start),(flush==4));
   s->block_start=(long)s->strstart;
   flush_pending(s->strm);
   if(s->strm->avail_out==0) return(flush==4 ? FINISH_STARTED : NEED_MORE);
//---
   return(flush==4 ? FINISH_DONE : BLOCK_DONE);
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
SZlib::block_state SZlib::deflate_fast(internal_state *s,int flush)
  {
   unsigned hash_head=0;
   int32_t      bflush;
//---
   for(;;)
     {
      //---
      if(s->lookahead<262)
        {
         fill_window(s);
         if(s->lookahead<262 && flush==0) return(NEED_MORE);
         if(s->lookahead==0) break;
        }
      //---
      if(s->lookahead>=3)
        {
         s->ins_h=uint32_t(((s->ins_h<<s->hash_shift)^s->window[s->strstart+2])&s->hash_mask);
         hash_head=s->prev[s->strstart&s->w_mask]=s->head[s->ins_h];
         s->head[s->ins_h]=(WORD)(s->strstart);
        }
      //---
      if(hash_head!=0 && s->strstart-hash_head<=(s->w_size-262))
        {
         if(s->strategy!=2 && s->strategy!=3)
            s->match_length=longest_match(s,hash_head);
         else
            if(s->strategy==3 && s->strstart - hash_head==1)
               s->match_length=longest_match_fast(s, hash_head);
        }
      //---
      if(s->match_length>=3)
        {
         BYTE len =BYTE(s->match_length-3);
         WORD dist=WORD(s->strstart-s->match_start);
         s->d_buf[s->last_lit]=dist;
         s->l_buf[s->last_lit++]=len;
         dist--;
         s->dyn_ltree[length_code[len]+257].fc.freq++;
         s->dyn_dtree[(dist<256 ? dist_code[dist] : dist_code[256+(dist>>7)])].fc.freq++;
         bflush=(s->last_lit==s->lit_bufsize-1);
         //---
         s->lookahead-=s->match_length;
         //---
         if(s->match_length<=s->max_lazy_match && s->lookahead>=3)
           {
            s->match_length--;
            do
              {
               s->strstart++;
               s->ins_h =uint32_t(((s->ins_h<<s->hash_shift)^s->window[s->strstart+2])&s->hash_mask);
               hash_head=s->prev[s->strstart & s->w_mask]=s->head[s->ins_h];
               s->head[s->ins_h]=(WORD)(s->strstart);
              }
            while(--s->match_length!=0);
            s->strstart++;
           }
         else
           {
            s->strstart+=s->match_length;
            s->match_length=0;
            s->ins_h=uint32_t(s->window[s->strstart]);
            s->ins_h=uint32_t(((s->ins_h<<s->hash_shift)^s->window[s->strstart+1])&s->hash_mask);
           }
        }
      else
        {
         BYTE cc=BYTE(s->window[s->strstart]);
         s->d_buf[s->last_lit]=0;
         s->l_buf[s->last_lit++]=cc;
         s->dyn_ltree[cc].fc.freq++;
         bflush=(s->last_lit==s->lit_bufsize-1);
         //---
         s->lookahead--;
         s->strstart++;
        }
      //---
      if(bflush)
        {
         _tr_flush_block(s,s->block_start>=0L?(char *)&s->window[s->block_start] : NULL,(uint32_t)((long)s->strstart-s->block_start),0);
         s->block_start=(long)s->strstart;
         flush_pending(s->strm);
         if(s->strm->avail_out==0) return(NEED_MORE);
        }
     }
//---
   _tr_flush_block(s,s->block_start>=0L?(char *)&s->window[s->block_start] : NULL,(uint32_t)((long)s->strstart-s->block_start),(flush==4));
   s->block_start=(long)s->strstart;
   flush_pending(s->strm);
   if(s->strm->avail_out==0) return(flush==4 ? FINISH_STARTED : NEED_MORE);
//---
   return(flush==4 ? FINISH_DONE : BLOCK_DONE);
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
SZlib::block_state SZlib::deflate_slow(internal_state *s,int flush)
  {
   unsigned hash_head=0;
   int32_t      bflush;
//---
   for(;;)
     {
      //---
      if(s->lookahead<262)
        {
         fill_window(s);
         if(s->lookahead<262 && flush==0) return(NEED_MORE);
         if(s->lookahead==0) break;
        }
      //---
      if(s->lookahead>=3)
        {
         s->ins_h=uint32_t(((s->ins_h<<s->hash_shift)^s->window[s->strstart+2])&s->hash_mask);
         hash_head=s->prev[s->strstart&s->w_mask]=s->head[s->ins_h];
         s->head[s->ins_h]=(WORD)(s->strstart);
        }
      //---
      s->prev_length=s->match_length,s->prev_match=s->match_start;
      s->match_length=2;
      //---
      if(hash_head!=0 && s->prev_length<s->max_lazy_match && s->strstart-hash_head<=(s->w_size-262))
        {
         if(s->strategy!=2 && s->strategy!=3)
            s->match_length=longest_match(s,hash_head);
         else
            if(s->strategy==3 && s->strstart-hash_head==1)
               s->match_length=longest_match_fast(s, hash_head);
         //---
         if(s->match_length<=5 && (s->strategy==1 || (s->match_length==3 && s->strstart-s->match_start>4096)))
            s->match_length=2;
        }
      //---
      if(s->prev_length>=3 && s->match_length<=s->prev_length)
        {
         uint32_t max_insert=s->strstart+s->lookahead-3;
           {
            BYTE len =BYTE(s->prev_length-3);
            WORD dist=WORD((s->strstart-1)-s->prev_match);
            s->d_buf[s->last_lit]=dist;
            s->l_buf[s->last_lit++]=len;
            dist--;
            s->dyn_ltree[length_code[len]+257].fc.freq++;
            s->dyn_dtree[(dist<256 ? dist_code[dist] : dist_code[256+(dist>>7)])].fc.freq++;
            bflush=(s->last_lit==s->lit_bufsize-1);
           };
         //---
         s->lookahead-=s->prev_length-1;
         s->prev_length-=2;
         //---
         do
           {
            if(++s->strstart<=max_insert)
              {
               s->ins_h=uint32_t(((s->ins_h<<s->hash_shift)^s->window[s->strstart+2])&s->hash_mask);
               hash_head=s->prev[(s->strstart) & s->w_mask]=s->head[s->ins_h];
               s->head[s->ins_h]=(WORD)(s->strstart);
              }
           }
         while(--s->prev_length!=0);
         //---
         s->match_available=0;
         s->match_length=3-1;
         s->strstart++;
         //---
         if(bflush)
           {
            _tr_flush_block(s,s->block_start>=0L?(char *)&s->window[(unsigned)s->block_start] : NULL,(uint32_t)((long)s->strstart-s->block_start),0);
            s->block_start=(long)s->strstart;
            flush_pending(s->strm);
            if(s->strm->avail_out==0) return(NEED_MORE);
           }
        }
      else
         if(s->match_available)
           {
            //---
            BYTE cc=(BYTE)s->window[s->strstart-1];
            s->d_buf[s->last_lit]=0;
            s->l_buf[s->last_lit++]=cc;
            s->dyn_ltree[cc].fc.freq++;
            bflush=(s->last_lit==s->lit_bufsize-1);
            //---
            if(bflush)
              {
               _tr_flush_block(s,s->block_start>=0L ? (char *)&s->window[(unsigned)s->block_start] : NULL,(uint32_t)((long)s->strstart-s->block_start),0);
               s->block_start=(long)s->strstart;
               flush_pending(s->strm);
              }
            //---
            s->strstart++;
            s->lookahead--;
            if(s->strm->avail_out==0) return(NEED_MORE);
           }
         else
           {
            s->match_available=1;
            s->strstart++;
            s->lookahead--;
           }
     }
//---
   if(s->match_available)
     {
      BYTE cc=(BYTE)s->window[s->strstart-1];
      s->d_buf[s->last_lit]=0;
      s->l_buf[s->last_lit++]=cc;
      s->dyn_ltree[cc].fc.freq++;
      bflush=(s->last_lit==s->lit_bufsize-1);
      //---
      s->match_available=0;
     }
//---
   _tr_flush_block(s,s->block_start>=0L?(char *)&s->window[s->block_start] : NULL,(uint32_t)((long)s->strstart-s->block_start),(flush==4));
   s->block_start=s->strstart;
   flush_pending(s->strm);
   if(s->strm->avail_out==0) return(flush==4 ? FINISH_STARTED : NEED_MORE);
//---
   return(flush==4 ? FINISH_DONE : BLOCK_DONE);
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
uint32_t SZlib::crc32_little(uint32_t crc,const BYTE *buf,unsigned len)
  {
   uint32_t    c;
   const uint32_t *buf4;
//---
   c=(uint32_t)crc;
   c=~c;
//---
   while(len && ((ptrdiff_t)buf&3))
     {
      c=crc_table[0][(c^*buf++)&0xff] ^(c>>8);
      len--;
     }
//---
   buf4=(const uint32_t*)buf;
//---
   while(len>=32)
     {
      c^=*buf4++;
      c=crc_table[3][c&0xff]^crc_table[2][(c>>8)&0xff]^crc_table[1][(c>>16)&0xff]^crc_table[0][c>>24];
      c^=*buf4++;
      c=crc_table[3][c&0xff]^crc_table[2][(c>>8)&0xff]^crc_table[1][(c>>16)&0xff]^crc_table[0][c>>24];
      c^=*buf4++;
      c=crc_table[3][c&0xff]^crc_table[2][(c>>8)&0xff]^crc_table[1][(c>>16)&0xff]^crc_table[0][c>>24];
      c^=*buf4++;
      c=crc_table[3][c&0xff]^crc_table[2][(c>>8)&0xff]^crc_table[1][(c>>16)&0xff]^crc_table[0][c>>24];
      c^=*buf4++;
      c=crc_table[3][c&0xff]^crc_table[2][(c>>8)&0xff]^crc_table[1][(c>>16)&0xff]^crc_table[0][c>>24];
      c^=*buf4++;
      c=crc_table[3][c&0xff]^crc_table[2][(c>>8)&0xff]^crc_table[1][(c>>16)&0xff]^crc_table[0][c>>24];
      c^= *buf4++;
      c=crc_table[3][c&0xff]^crc_table[2][(c>>8)&0xff]^crc_table[1][(c>>16)&0xff]^crc_table[0][c>>24];
      c^=*buf4++;
      c=crc_table[3][c&0xff]^crc_table[2][(c>>8)&0xff]^crc_table[1][(c>>16)&0xff]^crc_table[0][c>>24];
      len-=32;
     }
//---
   while(len>=4)
     {
      c^=*buf4++;
      c=crc_table[3][c&0xff]^crc_table[2][(c>>8)&0xff]^crc_table[1][(c>>16)&0xff]^crc_table[0][c>>24];
      len-=4;
     }
   buf=(const BYTE*)buf4;
//---
   if(len)
      do
     {
      c=crc_table[0][(c^*buf++)&0xff]^(c>>8);
     } while(--len);
//---
   c=~c;
//---
   return(c);
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
uint32_t SZlib::crc32_big(uint32_t crc,const BYTE  *buf,unsigned len)
  {
   uint32_t    c;
   const uint32_t *buf4;
//---
   c=((((uint32_t)crc)>>24)+((((uint32_t)crc)>>8)&0xff00)+ ((((uint32_t)crc)&0xff00)<<8)+((((uint32_t)crc)&0xff)<<24));
   c=~c;
//---
   while(len && ((ptrdiff_t)buf&3))
     {
      c=crc_table[4][(c>>24)^*buf++]^(c<<8);
      len--;
     }
//---
   buf4=(const uint32_t*)buf;
   buf4--;
//---
   while(len>=32)
     {
      c^=*++buf4;
      c=crc_table[4][c&0xff]^crc_table[5][(c>>8)&0xff]^crc_table[6][(c>>16)&0xff]^crc_table[7][c>>24];
      c^=*++buf4;
      c=crc_table[4][c&0xff]^crc_table[5][(c>>8)&0xff]^crc_table[6][(c>>16)&0xff]^crc_table[7][c>>24];
      c^=*++buf4;
      c=crc_table[4][c&0xff]^crc_table[5][(c>>8)&0xff]^crc_table[6][(c>>16)&0xff]^crc_table[7][c>>24];
      c^=*++buf4;
      c=crc_table[4][c&0xff]^crc_table[5][(c>>8)&0xff]^crc_table[6][(c>>16)&0xff]^crc_table[7][c>>24];
      c^=*++buf4;
      c=crc_table[4][c&0xff]^crc_table[5][(c>>8)&0xff]^crc_table[6][(c>>16)&0xff]^crc_table[7][c>>24];
      c^=*++buf4;
      c=crc_table[4][c&0xff]^crc_table[5][(c>>8)&0xff]^crc_table[6][(c>>16)&0xff]^crc_table[7][c>>24];
      c^=*++buf4;
      c=crc_table[4][c&0xff]^crc_table[5][(c>>8)&0xff]^crc_table[6][(c>>16)&0xff]^crc_table[7][c>>24];
      c^=*++buf4;
      c=crc_table[4][c&0xff]^crc_table[5][(c>>8)&0xff]^crc_table[6][(c>>16)&0xff]^crc_table[7][c>>24];
      len-=32;
     }
//---
   while(len>=4)
     {
      c^=*++buf4;
      c=crc_table[4][c&0xff]^crc_table[5][(c>>8)&0xff]^crc_table[6][(c>>16)&0xff]^crc_table[7][c>>24];
      len-=4;
     }
//---
   buf4++;
//---
   buf=(const BYTE*)buf4;
//---
   if(len)
      do
     {
      c=crc_table[4][(c>>24)^*buf++]^(c<<8);
     }
   while(--len);
//---
   c=~c;
//---
   return(uint32_t)((((c)>>24)+(((c)>>8)&0xff00)+(((c)&0xff00)<<8)+(((c)&0xff)<<24)));
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
uint32_t SZlib::crc32(uint32_t crc,const BYTE *buf,unsigned len)
  {
   if(buf==0) return(0);
//---
   if(sizeof(void *)==sizeof(ptrdiff_t))
     {
      uint32_t endian=1;
      if(*((BYTE *)(&endian))) return crc32_little(crc,buf,len);
      else                     return crc32_big(crc,buf,len);
     }
//---
   crc=crc^0xffffffffUL;
//---
   while(len>=8)
     {
      crc=crc_table[0][((int)crc^(*buf++))&0xff]^(crc>>8);
      crc=crc_table[0][((int)crc^(*buf++))&0xff]^(crc>>8);
      crc=crc_table[0][((int)crc^(*buf++))&0xff]^(crc>>8);
      crc=crc_table[0][((int)crc^(*buf++))&0xff]^(crc>>8);
      crc=crc_table[0][((int)crc^(*buf++))&0xff]^(crc>>8);
      crc=crc_table[0][((int)crc^(*buf++))&0xff]^(crc>>8);
      crc=crc_table[0][((int)crc^(*buf++))&0xff]^(crc>>8);
      crc=crc_table[0][((int)crc^(*buf++))&0xff]^(crc>>8);
      len-=8;
     }
//---
   if(len)
      do
     {
      crc=crc_table[0][((int)crc^(*buf++))&0xff]^(crc>>8);
     } while(--len);
//---
   return(crc^0xffffffffUL);
  }
//+------------------------------------------------------------------+
//| Deflate                                                          |
//+------------------------------------------------------------------+
int SZlib::deflateInit2(z_stream *strm,int level,int method,int windowBits,int memLevel,int strategy)
  {
   internal_state *s;
   int32_t             wrap=1;
   WORD           *overlay;
//---
   if(strm==NULL) return(-2);
//---
   if(strm->zalloc==NULL) strm->zalloc=zcalloc;
   if(strm->zfree ==NULL) strm->zfree =zcfree;
//---
   if(level==-1) level=6;
//---
   if(windowBits<0)     { wrap=0; windowBits=-windowBits; }
   else
      if(windowBits>15) { wrap=2; windowBits-=16;         }
//---
   if(memLevel<1 || memLevel>9 || method!=8 ||
      windowBits<8 || windowBits>15 || level<0 || level>9 || strategy<0 || strategy>4) return(-2);
//---
   if(windowBits==8) windowBits=9;
//---
   s=(internal_state*)(*(strm->zalloc))(1,(sizeof(internal_state)));
   if(s==NULL) return(-4);
//---
   strm->state=(internal_state*)s;
   s->strm=strm;
//---
   s->wrap=wrap;
   s->gzhead=0;
   s->w_bits=(uint32_t)windowBits;
   s->w_size=1<<s->w_bits;
   s->w_mask=s->w_size-1;
//---
   s->hash_bits =uint32_t(memLevel + 7);
   s->hash_size =1 << s->hash_bits;
   s->hash_mask =s->hash_size - 1;
   s->hash_shift=((s->hash_bits+3-1)/3);
//---
   s->window=(char*)(*(strm->zalloc))(s->w_size   ,sizeof(char)*2);
   s->prev  =(WORD*)(*(strm->zalloc))(s->w_size   ,sizeof(WORD));
   s->head  =(WORD*)(*(strm->zalloc))(s->hash_size,sizeof(WORD));
//---
   s->lit_bufsize =1<<(memLevel+6);
//---
   overlay=(WORD *)(*(strm->zalloc))(s->lit_bufsize,(sizeof(WORD)+2));
   s->pending_buf=(char*)overlay;
   s->pending_buf_size=(uint32_t)s->lit_bufsize*sizeof(WORD)+2L;
//---
   if(s->window==0 || s->prev==0 || s->head==0 || s->pending_buf==0)
     {
      s->status=666;
      deflateEnd(strm);
      return(-4);
     }
//---
   s->d_buf=overlay+s->lit_bufsize/sizeof(WORD);
   s->l_buf=(BYTE*)(s->pending_buf+(1+sizeof(WORD))*s->lit_bufsize);
//---
   s->level   =level;
   s->strategy=strategy;
   s->method  =(char)method;
//---
   return(deflateReset(strm));
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
int SZlib::deflate(z_stream *strm,int flush)
  {
   int32_t old_flush;
   internal_state *s;
//---
   if(strm==NULL || strm->state==NULL || flush>4 || flush<0) return(-2);
   s=strm->state;
//---
   if(strm->next_out==0 || (strm->next_in==0 && strm->avail_in!=0) || (s->status==666 && flush!=4))  return(-2);
//---
   if(strm->avail_out==NULL) return(-5);
   s->strm=strm;
//---
   old_flush=s->last_flush;
   s->last_flush=flush;
//---
   if(s->status==42)
     {
      if(s->wrap==2)
        {
         strm->adler=crc32(0L,0,0);
         s->pending_buf[s->pending++]=31;
         s->pending_buf[s->pending++]=(char)(139);
         s->pending_buf[s->pending++]=8;
         if(s->gzhead==NULL)
           {
            s->pending_buf[s->pending++]=0;
            s->pending_buf[s->pending++]=0;
            s->pending_buf[s->pending++]=0;
            s->pending_buf[s->pending++]=0;
            s->pending_buf[s->pending++]=0;
            s->pending_buf[s->pending++]=(s->level==9 ? 2 : (s->strategy>=2 || s->level<2 ? 4 : 0));
            s->pending_buf[s->pending++]=0x0b;
            s->status=113;
           }
         else
           {
            s->pending_buf[s->pending++]=((s->gzhead->text ? 1 : 0) + (s->gzhead->hcrc ? 2 : 0) + (s->gzhead->extra==0 ? 0 : 4) + (s->gzhead->name==0 ? 0 : 8) + (s->gzhead->comment==0 ? 0 : 16));
            s->pending_buf[s->pending++]=((char)(s->gzhead->time&0xff));
            s->pending_buf[s->pending++]=((char)((s->gzhead->time>>8)&0xff));
            s->pending_buf[s->pending++]=((char)((s->gzhead->time>>16)&0xff));
            s->pending_buf[s->pending++]=((char)((s->gzhead->time>>24)&0xff));
            s->pending_buf[s->pending++]=(s->level==9 ? 2 : (s->strategy>=2 || s->level<2 ? 4 : 0));
            s->pending_buf[s->pending++]=char(s->gzhead->os & 0xff);
            //---
            if(s->gzhead->extra!=NULL)
              {
               s->pending_buf[s->pending++]=char(s->gzhead->extra_len&0xff);
               s->pending_buf[s->pending++]=char((s->gzhead->extra_len>>8)&0xff);
              }
            //---
            if(s->gzhead->hcrc) strm->adler=crc32(strm->adler,(BYTE*)s->pending_buf,s->pending);
            s->gzindex=0;
            s->status=69;
           }
        }
      else
        {
         uint32_t header=(8+((s->w_bits-8)<<4))<<8;
         uint32_t level_flags;
         if(s->strategy>=2 || s->level<2) level_flags=0;
         else
            if(s->level<6)                level_flags=1;
            else
               if(s->level==6)            level_flags=2;
               else                       level_flags=3;
         //---
         header|=(level_flags<<6);
         //---
         if(s->strstart!=0) header|=0x20;
         header+=31-(header%31);
         s->status=113;
         putShortMSB(s,header);
         //---
         if(s->strstart!=0)
           {
            putShortMSB(s,(uint32_t)(strm->adler>>16));
            putShortMSB(s,(uint32_t)(strm->adler&0xffff));
           }
         strm->adler=adler32(0L, 0, 0);
        }
     }
//---
   if(s->status==69)
     {
      if(s->gzhead->extra!=NULL)
        {
         uint32_t beg=s->pending;
         //---
         while(s->gzindex<(s->gzhead->extra_len&0xffff))
           {
            //---
            if(s->pending==s->pending_buf_size)
              {
               if(s->gzhead->hcrc && s->pending>beg)
                  strm->adler=crc32(strm->adler,(BYTE*)(s->pending_buf+beg),s->pending-beg);
               flush_pending(strm);
               beg=s->pending;
               if(s->pending==s->pending_buf_size) break;
              }
            //---
            s->pending_buf[s->pending++]=(s->gzhead->extra[s->gzindex]);
            s->gzindex++;
           }
         //---
         if(s->gzhead->hcrc && s->pending>beg)
            strm->adler=crc32(strm->adler,(BYTE*)(s->pending_buf + beg),s->pending - beg);
         if(s->gzindex==s->gzhead->extra_len)
           {
            s->gzindex=0;
            s->status=73;
           }
        }
      else
         s->status=73;
     }
//---
   if(s->status==73)
     {
      if(s->gzhead->name!=NULL)
        {
         uint32_t beg=s->pending;
         int32_t val;
         do
           {
            if(s->pending==s->pending_buf_size)
              {
               if(s->gzhead->hcrc && s->pending>beg)
                  strm->adler=crc32(strm->adler,(BYTE*)(s->pending_buf+beg),s->pending-beg);
               //---
               flush_pending(strm);
               //---
               beg=s->pending;
               if(s->pending==s->pending_buf_size)
                 {
                  val=1;
                  break;
                 }
              }
            //---
            val=s->gzhead->name[s->gzindex++];
            s->pending_buf[s->pending++]=char(val);
           }
         while(val!=0);
         //---
         if(s->gzhead->hcrc && s->pending>beg)
            strm->adler=crc32(strm->adler,(BYTE*)(s->pending_buf+beg),s->pending-beg);
         //---
         if(val==0)
           {
            s->gzindex=0;
            s->status=91;
           }
        }
      else
         s->status=91;
     }
//---
   if(s->status==91)
     {
      if(s->gzhead->comment!=NULL)
        {
         uint32_t beg=s->pending;
         int32_t val;
         do
           {
            if(s->pending==s->pending_buf_size)
              {
               if(s->gzhead->hcrc && s->pending>beg)
                  strm->adler=crc32(strm->adler,(BYTE*)(s->pending_buf+beg),s->pending-beg);
               //---
               flush_pending(strm);
               beg=s->pending;
               if(s->pending==s->pending_buf_size)
                 {
                  val=1;
                  break;
                 }
              }
            //---
            val=s->gzhead->comment[s->gzindex++];
            s->pending_buf[s->pending++]=char(val);
           }
         while(val!=0);
         //---
         if(s->gzhead->hcrc && s->pending>beg)
            strm->adler=crc32(strm->adler,(BYTE*)(s->pending_buf + beg),s->pending-beg);
         if(val==0) s->status=103;
        }
      else s->status=103;
     }
//---
   if(s->status==103)
     {
      if(s->gzhead->hcrc)
        {
         //---
         if(s->pending+2>s->pending_buf_size) flush_pending(strm);
         //---
         if(s->pending+2<=s->pending_buf_size)
           {
            //---
            s->pending_buf[s->pending++]=((char)(strm->adler&0xff));
            s->pending_buf[s->pending++]=((char)((strm->adler>>8)&0xff));
            //---
            strm->adler=crc32(0L,0,0);
            s->status=113;
           }
        }
      else
         s->status=113;
     }
//---
   if(s->pending!=0)
     {
      flush_pending(strm);
      if(strm->avail_out==0)
        {
         s->last_flush=-1;
         return(0);
        }
     }
   else
      if(strm->avail_in==0 && flush<=old_flush && flush!=4)
         return(-5);
//---
   if(s->status==666 && strm->avail_in!=0)
      return(-5);
//---
   if(strm->avail_in!=0 || s->lookahead!=0 || (flush!=0 && s->status!=666))
     {
      block_state bstate;
      bstate=(*(configuration_table[s->level].func))(s, flush);
      //---
      if(bstate==FINISH_STARTED || bstate==FINISH_DONE) s->status=666;
      //---
      if(bstate==NEED_MORE || bstate==FINISH_STARTED)
        {
         if(strm->avail_out==0) s->last_flush=-1;
         return(0);
        }
      //---
      if(bstate==BLOCK_DONE)
        {
         if(flush==1) _tr_align(s);
         else
           {
            _tr_stored_block(s,NULL,0L,0);
            //---
            if(flush==3)
              {
               s->head[s->hash_size-1]=0;
               memset((char *)s->head,0,(unsigned)(s->hash_size-1)*sizeof(*s->head));
              }
           }
         //---
         flush_pending(strm);
         //---
         if(strm->avail_out==0)
           {
            s->last_flush=-1;
            return(0);
           }
        }
     }
//---
   if(flush  !=4) return(0);
   if(s->wrap<=0) return(1);
   if(s->wrap==2)
     {
      s->pending_buf[s->pending++]=((char)(strm->adler&0xff));
      s->pending_buf[s->pending++]=((char)((strm->adler>>8)&0xff));
      s->pending_buf[s->pending++]=((char)((strm->adler>>16)&0xff));
      s->pending_buf[s->pending++]=((char)((strm->adler>>24)&0xff));
      s->pending_buf[s->pending++]=((char)(strm->total_in&0xff));
      s->pending_buf[s->pending++]=((char)((strm->total_in>>8)&0xff));
      s->pending_buf[s->pending++]=((char)((strm->total_in>>16)&0xff));
      s->pending_buf[s->pending++]=((char)((strm->total_in>>24)&0xff));
     }
   else
     {
      putShortMSB(s,(uint32_t)(strm->adler>>16));
      putShortMSB(s,(uint32_t)(strm->adler&0xffff));
     }
   flush_pending(strm);
//---
   if(s->wrap>0) s->wrap=-s->wrap;
//---
   return(s->pending!=0?0:1);
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
int SZlib::deflateEnd(z_stream *strm)
  {
   int32_t status;
//---
   if(strm==NULL || strm->state==NULL) return(-2);
//---
   status=strm->state->status;
   if(status!=42 && status!=69 && status!=73 && status!=91 && status!=103 && status!=113 && status!=666)
      return(-2);
//---
   if(strm->state->pending_buf) (*(strm->zfree))((void*)(strm->state->pending_buf));
   if(strm->state->head)        (*(strm->zfree))((void*)(strm->state->head));
   if(strm->state->prev)        (*(strm->zfree))((void*)(strm->state->prev));
   if(strm->state->window)      (*(strm->zfree))((void*)(strm->state->window));
//---
   (*(strm->zfree))((void*)(strm->state));
//---
   strm->state=0;
//---
   return(status==113 ? -3 : 0);
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
void SZlib::flush_pending(z_stream *strm)
  {
   unsigned len=strm->state->pending;
   if(len>strm->avail_out) len=strm->avail_out;
//---
   if(len==0) return;
//---
   memcpy(strm->next_out,strm->state->pending_out,len);
//---
   strm->next_out          +=len;
   strm->state->pending_out+=len;
   strm->total_out         +=len;
   strm->avail_out         -=len;
   strm->state->pending    -=len;
//---
   if(strm->state->pending==0) strm->state->pending_out=strm->state->pending_buf;
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
int SZlib::deflateReset(z_stream *strm)
  {
   internal_state *s;
   if(strm==NULL || strm->state==NULL || strm->zalloc==NULL || strm->zfree==NULL) return(-2);
//---
   strm->total_in =strm->total_out=0;
   strm->data_type=2;
   s              =(internal_state*)strm->state;
   s->pending     =0;
   s->pending_out =s->pending_buf;
   if(s->wrap<0)  s->wrap=-s->wrap;
   s->status      =s->wrap ? 42 : 113;
   strm->adler    =s->wrap==2 ? crc32(0L,0,0) : adler32(0L,0,0);
   s->last_flush=0;
//---
   _tr_init(s);
   lm_init(s);
//---
   return(0);
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
void SZlib::lm_init(internal_state *s)
  {
   s->window_size=(uint32_t)2L*s->w_size;
   s->head[s->hash_size-1]=0;
   memset((char*)s->head,0,(unsigned)(s->hash_size-1)*sizeof(*s->head));
   s->max_lazy_match  =configuration_table[s->level].max_lazy;
   s->good_match      =configuration_table[s->level].good_length;
   s->nice_match      =configuration_table[s->level].nice_length;
   s->max_chain_length=configuration_table[s->level].max_chain;
   s->strstart        =0;
   s->block_start     =0L;
   s->lookahead       =0;
   s->match_length    =s->prev_length=3-1;
   s->match_available =0;
   s->ins_h           =0;
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
uint32_t SZlib::adler32(uint32_t adler,const BYTE *buf,uint32_t len)
  {
   uint32_t sum2;
   unsigned n;
//---
   if(buf==NULL) return(1L);
//---
   sum2=(adler>>16)&0xffff;
   adler&=0xffff;
//---
   if(len==1)
     {
      adler+=buf[0];
      if(adler>=65521UL) adler-=65521UL;
      sum2+=adler;
      if(sum2>=65521UL) sum2-=65521UL;
      return(adler|(sum2<<16));
     }
//---
   if(len<16)
     {
      //---
      while(len--) { adler+=*buf++; sum2+=adler; }
      if(adler>=65521UL) adler-=65521UL;
      //---
      sum2%=65521UL;
      return(adler|(sum2<<16));
     }
//---
   while(len>=5552)
     {
      len-=5552;
      n=5552/16;
      //---
      do
        {
         adler+=(buf)[0];       sum2+=adler;
         adler+=(buf)[0+1];     sum2+=adler;
         adler+=(buf)[0+2];     sum2+=adler;
         adler+=(buf)[0+2+1];   sum2+=adler;
         adler+=(buf)[0+4];     sum2+=adler;
         adler+=(buf)[0+4+1];   sum2+=adler;
         adler+=(buf)[0+4+2];   sum2+=adler;
         adler+=(buf)[0+4+2+1]; sum2+=adler;
         adler+=(buf)[8];       sum2+=adler;
         adler+=(buf)[8+1];     sum2+=adler;
         adler+=(buf)[8+2];     sum2+=adler;
         adler+=(buf)[8+2+1];   sum2+=adler;
         adler+=(buf)[8+4];     sum2+=adler;
         adler+=(buf)[8+4+1];   sum2+=adler;
         adler+=(buf)[8+4+2];   sum2+=adler;
         adler+=(buf)[8+4+2+1]; sum2+=adler;
         buf+=16;
        }
      while(--n);
      //---
      adler%=65521UL;
      sum2 %=65521UL;
     }
//---
   if(len)
     {
      while(len>=16)
        {
         len-=16;
         adler+=(buf)[0];       sum2+=adler;
         adler+=(buf)[0+1];     sum2+=adler;
         adler+=(buf)[0+2];     sum2+=adler;
         adler+=(buf)[0+2+1];   sum2+=adler;
         adler+=(buf)[0+4];     sum2+=adler;
         adler+=(buf)[0+4+1];   sum2+=adler;
         adler+=(buf)[0+4+2];   sum2+=adler;
         adler+=(buf)[0+4+2+1]; sum2+=adler;
         adler+=(buf)[8];       sum2+=adler;
         adler+=(buf)[8+1];     sum2+=adler;
         adler+=(buf)[8+2];     sum2+=adler;
         adler+=(buf)[8+2+1];   sum2+=adler;
         adler+=(buf)[8+4];     sum2+=adler;
         adler+=(buf)[8+4+1];   sum2+=adler;
         adler+=(buf)[8+4+2];   sum2+=adler;
         adler+=(buf)[8+4+2+1]; sum2+=adler;
         buf+=16;
        }
      //---
      while(len--) { adler+=*buf++; sum2+=adler; }
      //---
      adler%=65521UL;
      sum2 %=65521UL;
     }
//---
   return(adler|(sum2<<16));
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
void SZlib::putShortMSB(internal_state *s,uint32_t b)
  {
   s->pending_buf[s->pending++]=((char)(b>>8));
   s->pending_buf[s->pending++]=((char)(b&0xff));
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
void SZlib::init_block(internal_state *s)
  {
   int32_t n;
//---
   for(n=0;n<286;n++) s->dyn_ltree[n].fc.freq=0;
   for(n=0;n<30; n++) s->dyn_dtree[n].fc.freq=0;
   for(n=0;n<19; n++) s->bl_tree[n].fc.freq  =0;
//---
   s->dyn_ltree[256].fc.freq=1;
   s->opt_len=s->static_len=0L;
   s->last_lit=s->matches=0;
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
void SZlib::_tr_init(internal_state *s)
  {
   s->l_desc.dyn_tree  =s->dyn_ltree;
   s->l_desc.stat_desc =&static_l_desc;
   s->d_desc.dyn_tree  =s->dyn_dtree;
   s->d_desc.stat_desc =&static_d_desc;
   s->bl_desc.dyn_tree =s->bl_tree;
   s->bl_desc.stat_desc=&static_bl_desc;
   s->bi_buf           =0;
   s->bi_valid         =0;
   s->last_eob_len     =8;
//---
   init_block(s);
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
void SZlib::_tr_align(internal_state *s)
  {
   int32_t len=3;
//---
   if(s->bi_valid>(int)16-len)
     {
      int32_t val=1<<1;
      s->bi_buf|=(val<<s->bi_valid);
      //---
      s->pending_buf[s->pending++]=((char)(s->bi_buf&0xff));
      s->pending_buf[s->pending++]=((char)(s->bi_buf>>8));
      //---
      s->bi_buf=(WORD)val>>(16-s->bi_valid);
      s->bi_valid+=len-16;
     }
   else
     {
      s->bi_buf|=(1<<1)<<s->bi_valid;
      s->bi_valid+=len;
     }
//---
   len=static_ltree[256].dl.len;
   if(s->bi_valid>(int)16-len)
     {
      int32_t val=static_ltree[256].fc.code;
      s->bi_buf|=(val<<s->bi_valid);
      //---
      s->pending_buf[s->pending++]=((char)(s->bi_buf&0xff));
      s->pending_buf[s->pending++]=((char)(s->bi_buf>>8));
      //---
      s->bi_buf=(WORD)val>>(16-s->bi_valid);
      s->bi_valid+=(len-16);
     }
   else
     {
      s->bi_buf  |=(static_ltree[256].fc.code)<<s->bi_valid;
      s->bi_valid+=len;
     }
//---
   bi_flush(s);
//---
   if(1+s->last_eob_len+10-s->bi_valid<9)
     {
      len=3;
      if(s->bi_valid>(int)16-len)
        {
         int32_t val=1<<1;
         s->bi_buf|=(val<<s->bi_valid);
         //---
         s->pending_buf[s->pending++]=((char)(s->bi_buf&0xff));
         s->pending_buf[s->pending++]=((char)(s->bi_buf>>8));
         //---
         s->bi_buf=(WORD)val>>(16-s->bi_valid);
         s->bi_valid+=len-16;
        }
      else
        {
         s->bi_buf|=(1<<1)<<s->bi_valid;
         s->bi_valid+=len;
        }
      //---
      len=static_ltree[256].dl.len;
      if(s->bi_valid>(int)16-len)
        {
         int32_t val=static_ltree[256].fc.code;
         //---
         s->bi_buf|=(val<<s->bi_valid);
         //---
         s->pending_buf[s->pending++]=((char)((s->bi_buf) & 0xff));
         s->pending_buf[s->pending++]=((char)((WORD)(s->bi_buf) >> 8));
         //---
         s->bi_buf=(WORD)val>>(16-s->bi_valid);
         s->bi_valid+=len-16;
        }
      else
        {
         s->bi_buf|=(static_ltree[256].fc.code)<<s->bi_valid;
         s->bi_valid+=len;
        }
      //---
      bi_flush(s);
     }
//---
   s->last_eob_len=7;
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
void SZlib::bi_windup(internal_state *s)
  {
//---
   if(s->bi_valid>8)
     {
      s->pending_buf[s->pending++]=((char)(s->bi_buf&0xff));
      s->pending_buf[s->pending++]=((char)(s->bi_buf>>8));
     }
   else
      if(s->bi_valid>0) s->pending_buf[s->pending++]=((char)s->bi_buf);
//---
   s->bi_buf  =0;
   s->bi_valid=0;
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
void SZlib::copy_block(internal_state *s,const char *buf,unsigned len,int header)
  {
//---
   bi_windup(s);
//---
   s->last_eob_len=8;
//---
   if(header)
     {
      //---
      s->pending_buf[s->pending++]=((char)(((WORD)len)&0xff));
      s->pending_buf[s->pending++]=((char)((WORD)((WORD)len)>>8));
      //---
      s->pending_buf[s->pending++]=((char)(((WORD)~len)&0xff));
      s->pending_buf[s->pending++]=((char)((WORD)((WORD)~len)>>8));
     }
//---
   while(len--) s->pending_buf[s->pending++]=(*buf++);
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
void SZlib::compress_block(internal_state *s,const ct_data *ltree,const ct_data *dtree)
  {
   unsigned dist;
   int32_t lc;
   unsigned lx=0;
   unsigned lcode;
   int32_t extra;
//---
   if(s->last_lit!=0)
      do
     {
      dist=s->d_buf[lx];
      lc=s->l_buf[lx++];
      if(dist==0)
        {
         int32_t len=ltree[lc].dl.len;
         if(s->bi_valid>(int)16-len)
           {
            int32_t val=ltree[lc].fc.code;
            s->bi_buf|=(val<<s->bi_valid);
            //---
            s->pending_buf[s->pending++]=((char)(s->bi_buf&0xff));
            s->pending_buf[s->pending++]=((char)(s->bi_buf>>8));
            //---
            s->bi_buf=(WORD)val>>(16-s->bi_valid);
            s->bi_valid+=len-16;
           }
         else
           {
            s->bi_buf|=(ltree[lc].fc.code)<<s->bi_valid;
            s->bi_valid+=len;
           }
        }
      else
        {
         lcode=length_code[lc];
         int32_t len=ltree[lcode+257].dl.len;
         if(s->bi_valid>(int)16-len)
           {
            int32_t val=ltree[lcode+257].fc.code;
            s->bi_buf|=(val<<s->bi_valid);
            //---
            s->pending_buf[s->pending++]=((char)(s->bi_buf&0xff));
            s->pending_buf[s->pending++]=((char)(s->bi_buf>>8));
            //---
            s->bi_buf=(WORD)val>>(16-s->bi_valid);
            s->bi_valid+=len-16;
           }
         else
           {
            s->bi_buf|=(ltree[lcode+257].fc.code)<<s->bi_valid;
            s->bi_valid+=len;
           }
         //---
         extra=extra_lbits[lcode];
         if(extra!=0)
           {
            lc-=base_length[lcode];
            //---
            int32_t len2=extra;
            if(s->bi_valid>(int)16-len2)
              {
               int32_t val=lc;
               s->bi_buf|=(val<<s->bi_valid);
               //---
               s->pending_buf[s->pending++]=((char)(s->bi_buf&0xff));
               s->pending_buf[s->pending++]=((char)(s->bi_buf>>8));
               //---
               s->bi_buf=(WORD)val>>(16-s->bi_valid);
               s->bi_valid+=len2-16;
              }
            else
              {
               s->bi_buf|=(lc)<<s->bi_valid;
               s->bi_valid+=len2;
              }
           }
         //---
         dist--;
         lcode=(dist<256 ? dist_code[dist] : dist_code[256+((dist)>>7)]);
         //---
         len=dtree[lcode].dl.len;
         if(s->bi_valid>(int)16-len)
           {
            int32_t val=dtree[lcode].fc.code;
            s->bi_buf|=(val<<s->bi_valid);
            //---
            s->pending_buf[s->pending++]=((char)(s->bi_buf&0xff));
            s->pending_buf[s->pending++]=((char)(s->bi_buf>>8));
            //---
            s->bi_buf=(WORD)val>>(16-s->bi_valid);
            s->bi_valid+=len-16;
           }
         else
           {
            s->bi_buf|=(dtree[lcode].fc.code)<<s->bi_valid;
            s->bi_valid+=len;
           }
         //---
         extra=extra_dbits[lcode];
         //---
         if(extra!=0)
           {
            dist-=base_dist[lcode];
            //---
            int32_t len2=extra;
            if(s->bi_valid>(int)16-len2)
              {
               int32_t val=(int)dist;
               s->bi_buf|=(val<<s->bi_valid);
               //---
               s->pending_buf[s->pending++]=((char)(s->bi_buf&0xff));
               s->pending_buf[s->pending++]=((char)(s->bi_buf>>8));
               //---
               s->bi_buf=(WORD)val>>(16-s->bi_valid);
               s->bi_valid+=len2-16;
              }
            else
              {
               s->bi_buf|=(dist)<<s->bi_valid;
               s->bi_valid+=len2;
              }
           }
        }
     }
   while(lx<s->last_lit);
//---
   int32_t len=ltree[256].dl.len;
   if(s->bi_valid>(int)16-len)
     {
      int32_t val=ltree[256].fc.code;
      s->bi_buf|=(val<<s->bi_valid);
      //---
      s->pending_buf[s->pending++]=((char)(s->bi_buf&0xff));
      s->pending_buf[s->pending++]=((char)(s->bi_buf>>8));
      //---
      s->bi_buf   =(WORD)val>>(16-s->bi_valid);
      s->bi_valid+=len-16;
     }
   else
     {
      s->bi_buf  |=(ltree[256].fc.code)<<s->bi_valid;
      s->bi_valid+=len;
     }
//---
   s->last_eob_len=ltree[256].dl.len;
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
void SZlib::set_data_type(const internal_state *s)
  {
   int32_t n;
//---
   for(n=0;n<9;n++)
      if(s->dyn_ltree[n].fc.freq!=0) break;
//---
   if(n==9)
      for(n=14;n<32; n++)
         if(s->dyn_ltree[n].fc.freq!=0) break;
//---
   s->strm->data_type=(n==32) ? 1 : 0;
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
void SZlib::send_tree(internal_state *s,const ct_data *tree,int max_code)
  {
   int32_t n,prevlen=-1,curlen,nextlen=tree[0].dl.len,count=0,max_count=7,min_count=4;
//---
   if(nextlen==0) max_count=138,min_count=3;
//---
   for(n=0;n<=max_code;n++)
     {
      curlen=nextlen;
      nextlen=tree[n+1].dl.len;
      //---
      if(++count<max_count && curlen==nextlen) continue;
      else
         if(count<min_count)
           {
            do
              {
               int32_t len=s->bl_tree[curlen].dl.len;
               if(s->bi_valid>(int)(8 * 2) - len)
                 {
                  int32_t val=s->bl_tree[curlen].fc.code;
                  s->bi_buf |= (val << s->bi_valid);
                  //---
                  s->pending_buf[s->pending++]=((char)(s->bi_buf&0xff));
                  s->pending_buf[s->pending++]=((char)(s->bi_buf>>8));
                  //---
                  s->bi_buf=(WORD)val>>(16-s->bi_valid);
                  s->bi_valid+=len-16;
                 }
               else
                 {
                  s->bi_buf|=(s->bl_tree[curlen].fc.code)<<s->bi_valid;
                  s->bi_valid+=len;
                 }
              }
            while(--count!=0);
           }
         else
            if(curlen!=0)
              {
               if(curlen!=prevlen)
                 {
                  int32_t len=s->bl_tree[curlen].dl.len;
                  if(s->bi_valid>(int)16-len)
                    {
                     int32_t val=s->bl_tree[curlen].fc.code;
                     s->bi_buf|=(val<<s->bi_valid);
                     //---
                     s->pending_buf[s->pending++]=((char)(s->bi_buf&0xff));
                     s->pending_buf[s->pending++]=((char)(s->bi_buf>>8));
                     //---
                     s->bi_buf=(WORD)val>>(16-s->bi_valid);
                     s->bi_valid+=len-16;
                    }
                  else
                    {
                     s->bi_buf|=(s->bl_tree[curlen].fc.code)<<s->bi_valid;
                     s->bi_valid+=len;
                    }
                  //---
                  count--;
                 }
               //---
               int32_t len=s->bl_tree[16].dl.len;
               if(s->bi_valid>(int)16-len)
                 {
                  int32_t val=s->bl_tree[16].fc.code;
                  s->bi_buf|=(val<<s->bi_valid);
                  //---
                  s->pending_buf[s->pending++]=((char)(s->bi_buf&0xff));
                  s->pending_buf[s->pending++]=((char)(s->bi_buf>>8));
                  //---
                  s->bi_buf=(WORD)val>>(16-s->bi_valid);
                  s->bi_valid+=len-16;
                 }
               else
                 {
                  s->bi_buf|=(s->bl_tree[16].fc.code)<<s->bi_valid;
                  s->bi_valid+=len;
                 }
               //---
               len=2;
               if(s->bi_valid>(int)16-len)
                 {
                  int32_t val=count-3;
                  s->bi_buf|=(val<<s->bi_valid);
                  //---
                  s->pending_buf[s->pending++]=((char)((s->bi_buf)&0xff));
                  s->pending_buf[s->pending++]=((char)((WORD)(s->bi_buf)>>8));
                  //---
                  s->bi_buf=(WORD)val>>(16-s->bi_valid);
                  s->bi_valid+=len-16;
                 }
               else
                 {
                  s->bi_buf|=(count-3)<<s->bi_valid;
                  s->bi_valid+=len;
                 }
               //---
              }
            else
               if(count<=10)
                 {
                  int32_t len=s->bl_tree[17].dl.len;
                  if(s->bi_valid>(int)16-len)
                    {
                     int32_t val=s->bl_tree[17].fc.code;
                     s->bi_buf|=(val<<s->bi_valid);
                     //---
                     s->pending_buf[s->pending++]=((char)(s->bi_buf&0xff));
                     s->pending_buf[s->pending++]=((char)(s->bi_buf>>8));
                     //---
                     s->bi_buf=(WORD)val>>(16-s->bi_valid);
                     s->bi_valid+=len-16;
                    }
                  else
                    {
                     s->bi_buf|=(s->bl_tree[17].fc.code)<<s->bi_valid;
                     s->bi_valid+=len;
                    }
                  //---
                  len=3;
                  if(s->bi_valid>(int)16-len)
                    {
                     int32_t val=count-3;
                     s->bi_buf|=(val<<s->bi_valid);
                     //---
                     s->pending_buf[s->pending++]=((char)(s->bi_buf&0xff));
                     s->pending_buf[s->pending++]=((char)(s->bi_buf>>8));
                     //---
                     s->bi_buf=(WORD)val>>(16-s->bi_valid);
                     s->bi_valid+=(len-16);
                    }
                  else
                    {
                     s->bi_buf|=(count-3)<<s->bi_valid;
                     s->bi_valid+=len;
                    }
                 }
               else
                 {
                  int32_t len=s->bl_tree[18].dl.len;
                  if(s->bi_valid>(int)16-len)
                    {
                     int32_t val=s->bl_tree[18].fc.code;
                     s->bi_buf|=(val<<s->bi_valid);
                     //---
                     s->pending_buf[s->pending++]=((char)(s->bi_buf&0xff));
                     s->pending_buf[s->pending++]=((char)(s->bi_buf>>8));
                     //---
                     s->bi_buf=(WORD)val>>(16-s->bi_valid);
                     s->bi_valid+=len-16;
                    }
                  else
                    {
                     s->bi_buf|=(s->bl_tree[18].fc.code)<<s->bi_valid;
                     s->bi_valid+=len;
                    }
                  //---
                  len=7;
                  if(s->bi_valid>(int)16-len)
                    {
                     int32_t val=count-11;
                     s->bi_buf|=(val<<s->bi_valid);
                     //---
                     s->pending_buf[s->pending++]=((char)(s->bi_buf&0xff));
                     s->pending_buf[s->pending++]=((char)(s->bi_buf>>8));
                     //---
                     s->bi_buf=(WORD)val>>(16-s->bi_valid);
                     s->bi_valid+=len-16;
                    }
                  else
                    {
                     s->bi_buf|=(count-11)<<s->bi_valid;
                     s->bi_valid+=len;
                    }
                 }
      //---
      count=0; prevlen=curlen;
      //---
      if(nextlen==0)         { max_count=138; min_count=3; }
      else
         if(curlen==nextlen) { max_count=6;   min_count=3; }
         else                { max_count=7;   min_count=4; }
     }
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
void SZlib::send_all_trees(internal_state *s, int lcodes, int dcodes,int blcodes)
  {
   int32_t rank,len;
//---
   len=5;
   if(s->bi_valid>(int)16-len)
     {
      int32_t val=lcodes-257;
      s->bi_buf|=(val<<s->bi_valid);
      //---
      s->pending_buf[s->pending++]=((char)(s->bi_buf&0xff));
      s->pending_buf[s->pending++]=((char)(s->bi_buf>>8));
      //---
      s->bi_buf=(WORD)val>>(16-s->bi_valid);
      s->bi_valid+=len-16;
     }
   else
     {
      s->bi_buf|=(lcodes-257)<<s->bi_valid;
      s->bi_valid+=len;
     }
//---
   len=5;
   if(s->bi_valid>(int)16-len)
     {
      int32_t val=dcodes-1;
      s->bi_buf|=(val<<s->bi_valid);
      //---
      s->pending_buf[s->pending++]=((char)(s->bi_buf&0xff));
      s->pending_buf[s->pending++]=((char)(s->bi_buf>>8));
      //---
      s->bi_buf=(WORD)val>>(16-s->bi_valid);
      s->bi_valid+=len-16;
     }
   else
     {
      s->bi_buf|=(dcodes-1)<<s->bi_valid;
      s->bi_valid+=len;
     }
//---
   len=4;
   if(s->bi_valid>(int)16-len)
     {
      int32_t val=blcodes-4;
      s->bi_buf|=(val<<s->bi_valid);
      //---
      s->pending_buf[s->pending++]=((char)(s->bi_buf&0xff));
      s->pending_buf[s->pending++]=((char)(s->bi_buf>>8));
      //---
      s->bi_buf=(WORD)val>>(16-s->bi_valid);
      s->bi_valid+=len-16;
     }
   else
     {
      s->bi_buf|=(blcodes-4)<<s->bi_valid;
      s->bi_valid+=len;
     }
//---
   for(rank=0;rank<blcodes;rank++)
     {
      len=3;
      if(s->bi_valid>(int)16-len)
        {
         int32_t val=s->bl_tree[bl_order[rank]].dl.len;
         s->bi_buf|=(val<<s->bi_valid);
         //---
         s->pending_buf[s->pending++]=((char)(s->bi_buf&0xff));
         s->pending_buf[s->pending++]=((char)(s->bi_buf>>8));
         //---
         s->bi_buf=(WORD)val>>(16-s->bi_valid);
         s->bi_valid+=len-16;
        }
      else
        {
         s->bi_buf|=(s->bl_tree[bl_order[rank]].dl.len)<<s->bi_valid;
         s->bi_valid+=len;
        }
     }
//---
   send_tree(s,(ct_data *)s->dyn_ltree,lcodes-1);
   send_tree(s,(ct_data *)s->dyn_dtree,dcodes-1);
  }

//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
void SZlib::scan_tree(internal_state *s,ct_data *tree,int max_code)
  {
   int32_t n,prevlen=-1,curlen,nextlen=tree[0].dl.len,count=0,max_count=7,min_count=4;
//---
   if(nextlen==0) { max_count=138; min_count=3; }
//---
   tree[max_code+1].dl.len=(WORD)0xffff;
//---
   for(n=0;n<=max_code;n++)
     {
      curlen=nextlen;
      nextlen=tree[n+1].dl.len;
      //---
      if(++count<max_count && curlen==nextlen) continue;
      //---
      else
         if(count<min_count) s->bl_tree[curlen].fc.freq+=count;
         else
            if(curlen!=0)
              {
               if(curlen!=prevlen) s->bl_tree[curlen].fc.freq++;
               s->bl_tree[16].fc.freq++;
              }
            else
               if(count<=10) s->bl_tree[17].fc.freq++;
               else               s->bl_tree[18].fc.freq++;
      //---
      count=0;
      prevlen=curlen;
      //---
      if(nextlen==0)         { max_count=138; min_count=3; }
      else
         if(curlen==nextlen) { max_count=6;   min_count=3; }
         else                { max_count=7;   min_count=4; }
     }
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
void SZlib::pqdownheap(internal_state *s,const ct_data *tree,int k)
  {
   int32_t v=s->heap[k];
   int32_t j=k<<1;
//---
   while(j<=s->heap_len)
     {
      if(j<s->heap_len &&
         (tree[s->heap[j+1]].fc.freq<tree[s->heap[j]].fc.freq ||
         (tree[s->heap[j+1]].fc.freq==tree[s->heap[j]].fc.freq && s->depth[s->heap[j+1]]<=s->depth[s->heap[j]]))) j++;
      //---
      if((tree[v].fc.freq<tree[s->heap[j]].fc.freq ||
         (tree[v].fc.freq==tree[s->heap[j]].fc.freq && s->depth[v]<=s->depth[s->heap[j]]))) break;
      //---
      s->heap[k]=s->heap[j];
      k=j;
      //---
      j<<=1;
     }
//---
   s->heap[k]=v;
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
void SZlib::gen_bitlen(internal_state *s,const tree_desc *desc)
  {
   ct_data       *tree      =desc->dyn_tree;
   int32_t            max_code  =desc->max_code;
   const ct_data *stree     =desc->stat_desc->static_tree;
   const int32_t     *extra     =desc->stat_desc->extra_bits;
   int32_t            base      =desc->stat_desc->extra_base;
   int32_t            max_length=desc->stat_desc->max_length;
   int32_t            h,n,m,bits,xbits;
   WORD           f;
   int32_t            overflow=0;
//---
   for(bits=0;bits<=15;bits++) s->bl_count[bits]=0;
//---
   tree[s->heap[s->heap_max]].dl.len=0;
//---
   for(h=s->heap_max+1;h<573;h++)
     {
      n=s->heap[h];
      bits=tree[tree[n].dl.dad].dl.len+1;
      //---
      if(bits>max_length) bits=max_length, overflow++;
      //---
      tree[n].dl.len=(WORD)bits;
      //---
      if(n>max_code) continue;
      //---
      s->bl_count[bits]++;
      xbits=0;
      if(n>=base) xbits=extra[n-base];
      f=tree[n].fc.freq;
      s->opt_len+=(uint32_t)f * (bits + xbits);
      if(stree) s->static_len+=(uint32_t)f*(stree[n].dl.len+xbits);
     }
//---
   if(overflow==0) return;
//---
   do
     {
      bits=max_length-1;
      while(s->bl_count[bits]==0) bits--;
      s->bl_count[bits]--;
      s->bl_count[bits+1]+=2;
      s->bl_count[max_length]--;
      overflow-=2;
     }
   while(overflow>0);
//---
   for(bits=max_length; bits!=0; bits--)
     {
      n=s->bl_count[bits];
      while(n!=0)
        {
         m=s->heap[--h];
         //---
         if(m>max_code) continue;
         //---
         if((unsigned) tree[m].dl.len!=(unsigned)bits)
           {
            s->opt_len+=((long)bits-(long)tree[m].dl.len)*(long)tree[m].fc.freq;
            tree[m].dl.len=(WORD)bits;
           }
         //---
         n--;
        }
     }
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
unsigned SZlib::bi_reverse(unsigned lcode,int len)
  {
   uint32_t res=0;
//---
   do
     {
      res|=lcode&1;
      lcode>>=1,res<<=1;
     }
   while(--len>0);
//---
   return(res>>1);
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
void SZlib::gen_codes(ct_data *tree,int max_code,const WORD *bl_count)
  {
   WORD next_code[16],lcode=0;
   int32_t  bits,n;
//---
   for(bits=1;bits<=15;bits++)
      next_code[bits]=lcode=WORD((lcode+bl_count[bits-1])<<1);
//---
   for(n=0;n<=max_code;n++)
     {
      int32_t len=tree[n].dl.len;
      if(len==0) continue;
      //---
      tree[n].fc.code=(WORD)bi_reverse(next_code[len]++,len);
     }
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
void SZlib::build_tree(internal_state *s,tree_desc *desc)
  {
   ct_data       *tree =desc->dyn_tree;
   const ct_data *stree=desc->stat_desc->static_tree;
   int32_t            elems=desc->stat_desc->elems;
   int32_t            n,m,node,max_code=-1;
//---
   s->heap_len=0;
   s->heap_max=573;
//---
   for(n=0;n<elems;n++)
     {
      if(tree[n].fc.freq!=0)
        {
         s->heap[++(s->heap_len)]=max_code=n;
         s->depth[n]=0;
        }
      else tree[n].dl.len=0;
     }
//---
   while(s->heap_len<2)
     {
      node=s->heap[++(s->heap_len)]=(max_code<2 ? ++max_code:0);
      tree[node].fc.freq=1;
      s->depth[node]=0;
      s->opt_len--;
      if(stree)s->static_len-=stree[node].dl.len;
     }
//---
   desc->max_code=max_code;
//---
   for(n=s->heap_len/2;n>=1;n--) pqdownheap(s,tree,n);
//---
   node=elems;
//---
   do
     {
      n=s->heap[1];
      s->heap[1]=s->heap[s->heap_len--];
      pqdownheap(s,tree,1);
      //---
      m=s->heap[1];
      s->heap[--(s->heap_max)]=n;
      s->heap[--(s->heap_max)]=m;
      //---
      tree[node].fc.freq=tree[n].fc.freq + tree[m].fc.freq;
      s->depth[node]=(BYTE)((s->depth[n]>=s->depth[m] ? s->depth[n] : s->depth[m]) + 1);
      //---
      tree[n].dl.dad=tree[m].dl.dad=(WORD)node;
      //---
      s->heap[1]=node++;
      pqdownheap(s,tree,1);
     }
   while(s->heap_len>=2);
//---
   s->heap[--(s->heap_max)]=s->heap[1];
//---
   gen_bitlen(s,desc);
   gen_codes(tree,max_code,s->bl_count);
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
int SZlib::build_bl_tree(internal_state *s)
  {
   int32_t max_blindex;
//---
   scan_tree(s,s->dyn_ltree,s->l_desc.max_code);
   scan_tree(s,s->dyn_dtree,s->d_desc.max_code);
//---
   build_tree(s,&s->bl_desc);
//---
   for(max_blindex=18;max_blindex>=3;max_blindex--)
      if(s->bl_tree[bl_order[max_blindex]].dl.len!=0) break;
//---
   s->opt_len+=3*(max_blindex+1)+14;
//---
   return(max_blindex);
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
void SZlib::_tr_stored_block(internal_state *s,const char* buf,uint32_t stored_len,int eof)
  {
//---
   int32_t len=3;
   if(s->bi_valid>(int)16-len)
     {
      int32_t val=(0<<1)+eof;
      s->bi_buf|=(val<<s->bi_valid);
      //---
      s->pending_buf[s->pending++]=((char)((s->bi_buf)&0xff));
      s->pending_buf[s->pending++]=((char)((WORD)(s->bi_buf)>>8));
      //---
      s->bi_buf=(WORD)val>>(16-s->bi_valid);
      s->bi_valid+=len-16;
     }
   else
     {
      s->bi_buf|=((0<<1)+eof)<<s->bi_valid;
      s->bi_valid+=len;
     }
//---
   copy_block(s,buf,stored_len,1);
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
void SZlib::_tr_flush_block(internal_state *s,char *buf,uint32_t stored_len,int eof)
  {
   uint32_t  opt_lenb,static_lenb;
   int32_t   max_blindex=0;
//---
   if(s->level>0)
     {
      if(stored_len>0 && s->strm->data_type==2) set_data_type(s);
      //---
      build_tree(s,(&s->l_desc));
      build_tree(s,(&s->d_desc));
      //---
      max_blindex=build_bl_tree(s);
      //---
      opt_lenb   =(s->opt_len+10)>>3;
      static_lenb=(s->static_len+10)>>3;
      //---
      if(static_lenb<=opt_lenb) opt_lenb=static_lenb;
     }
   else opt_lenb=static_lenb=stored_len+5;
//---
   if(stored_len+4<=opt_lenb && buf!=NULL) _tr_stored_block(s,buf,stored_len,eof);
   else
      if(s->strategy==4 || static_lenb==opt_lenb)
        {
         int32_t len=3;
         if(s->bi_valid>(int)16-len)
           {
            int32_t val=(1<<1)+eof;
            s->bi_buf|=(val<<s->bi_valid);
            //---
            s->pending_buf[s->pending++]=((char)((s->bi_buf)&0xff));
            s->pending_buf[s->pending++]=((char)((WORD)(s->bi_buf)>>8));
            //---
            s->bi_buf=(WORD)val>>(16-s->bi_valid);
            s->bi_valid+=len-16;
           }
         else
           {
            s->bi_buf|=((1<<1)+eof)<<s->bi_valid;
            s->bi_valid+=len;
           }
         //---
         compress_block(s,(ct_data*)static_ltree,(ct_data*)static_dtree);
        }
      else
        {
         int32_t len=3;
         if(s->bi_valid>(int)16-len)
           {
            int32_t val=(2<<1)+eof;
            s->bi_buf|=(val<<s->bi_valid);
            //---
            s->pending_buf[s->pending++]=((char)(s->bi_buf&0xff));
            s->pending_buf[s->pending++]=((char)(s->bi_buf>>8));
            //---
            s->bi_buf=(WORD)val>>(16-s->bi_valid);
            s->bi_valid+=len-16;
           }
         else
           {
            s->bi_buf|=((2<<1)+eof)<<s->bi_valid;
            s->bi_valid+=len;
           }
         //---
         send_all_trees(s,s->l_desc.max_code+1, s->d_desc.max_code+1,max_blindex+1);
         //---
         compress_block(s,(ct_data *)s->dyn_ltree,(ct_data *)s->dyn_dtree);
        }
//---
   init_block(s);
//---
   if(eof) bi_windup(s);
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
void SZlib::bi_flush(internal_state *s)
  {
   if(s->bi_valid==16)
     {
      //---
      s->pending_buf[s->pending++]=((char)(s->bi_buf&0xff));
      s->pending_buf[s->pending++]=((char)(s->bi_buf>>8));
      //---
      s->bi_buf=0;
      s->bi_valid=0;
     }
   else
      if(s->bi_valid>=8)
        {
         s->pending_buf[s->pending++]=((char)s->bi_buf);
         s->bi_buf>>=8;
         s->bi_valid-=8;
        }
  }
//+------------------------------------------------------------------+
//| Inflate                                                          |
//+------------------------------------------------------------------+
int SZlib::inflateInit2(z_stream *strm,int windowBits)
  {
   inflate_state *state;
//---
   if(strm==NULL) return(-2);
//---
   if(strm->zalloc==NULL) strm->zalloc=zcalloc;
   if(strm->zfree ==NULL) strm->zfree =zcfree;
//---
   state=(inflate_state*)(*(strm->zalloc))(1,sizeof(inflate_state));
   if(state==NULL) return(-4);
//---
   strm->state=(internal_state*)state;
//---
   if(windowBits<0)
     {
      state->wrap=0;
      windowBits=-windowBits;
     }
   else
     {
      state->wrap=(windowBits>>4)+1;
      if(windowBits<48) windowBits&=15;
     }
//---
   if(windowBits<8 || windowBits>15)
     {
      (*(strm->zfree))(state);
      strm->state=0;
      return(-2);
     }
//---
   state->wbits=(unsigned)windowBits;
   state->window=0;
//---
   return inflateReset(strm);
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
void SZlib::inflate_fast(z_stream *strm, unsigned start)
  {
   inflate_state  *state;
   BYTE           *in,*last,*out,*beg,*end,*window,*from;
   unsigned        wsize,whave,write,bits,lmask,dmask,op,len,dist;
   uint32_t        hold;
   code const     *lcode;
   code const     *dcode;
   code            thisblock;
//---
   state =(inflate_state*)strm->state;
   in    =(BYTE*)(strm->next_in-1);
   last  =in+(strm->avail_in-5);
   out   =(BYTE*)(strm->next_out-1);
   beg   =out-(start-strm->avail_out);
   end   =out+(strm->avail_out-257);
   wsize =state->wsize;
   whave =state->whave;
   write =state->write;
   window=state->window;
   hold  =state->hold;
   bits  =state->bits;
   lcode =state->lencode;
   dcode =state->distcode;
   lmask =(1U<<state->lenbits)-1;
   dmask =(1U<<state->distbits)-1;
//---
   do
     {
      //---
      if(bits<15)
        {
         hold+=(uint32_t)(*++(in))<<bits; bits+=8;
         hold+=(uint32_t)(*++(in))<<bits; bits+=8;
        }
      //---
      thisblock=lcode[hold&lmask];
      dolen:
      op=(unsigned)(thisblock.bits);
      hold>>=op;
      bits-=op;
      op=(unsigned)(thisblock.op);
      //---
      if(op==0) *++(out)=(BYTE)(thisblock.val);
      else
         if(op&16)
           {
            len=(unsigned)(thisblock.val);
            op&=15;
            //---
            if(op)
              {
               //---
               if(bits<op)
                 {
                  hold+=(uint32_t)(*++(in))<<bits; bits+=8;
                 }
               //---
               len+=(unsigned)hold&((1U<<op)-1);
               hold>>=op;
               bits-=op;
              }
            //---
            if(bits<15)
              {
               hold+=(uint32_t)(*++(in))<<bits; bits+=8;
               hold+=(uint32_t)(*++(in))<<bits; bits+=8;
              }
            //---
            thisblock=dcode[hold & dmask];
            dodist:
            op=(unsigned)(thisblock.bits);
            hold>>=op;
            bits -=op;
            op    =(unsigned)(thisblock.op);
            //---
            if(op&16)
              {
               dist=(unsigned)(thisblock.val);
               op&=15;
               if(bits<op)
                 {
                  hold+=(uint32_t)(*++(in))<<bits; bits+=8;
                  if(bits<op)
                    {
                     hold+=(uint32_t)(*++(in))<<bits; bits+=8;
                    }
                 }
               //---
               dist+=(unsigned)hold&((1U <<op)-1);
               hold >>= op;
               bits-=op;
               //---
               op=(unsigned)(out-beg);
               if(dist>op)
                 {
                  op=dist-op;
                  if(op>whave) { state->mode=BAD; break; }
                  //---
                  from=window-1;
                  //---
                  if(write==0)
                    {
                     from+=wsize-op;
                     if(op<len)
                       {
                        len-=op;
                        do { *++(out)=*++(from); }while(--op);
                        //---
                        from=out-dist;
                       }
                    }
                  else
                     if(write<op)
                       {
                        from+=wsize+write-op;
                        op-=write;
                        if(op<len)
                          {
                           len-=op;
                           do { *++(out)=*++(from); } while(--op);
                           //---
                           from=window-1;
                           //---
                           if(write<len)
                             {
                              op  =write;
                              len-=op;
                              //---
                              do { *++(out)=*++(from); } while(--op);
                              //---
                              from=out-dist;
                             }
                          }
                       }
                     else
                       {
                        from+=write-op;
                        //---
                        if(op<len)
                          {
                           len-=op;
                           do { *++(out)=*++(from); } while(--op);
                           //---
                           from=out - dist;
                          }
                       }
                  //---
                  while(len>2)
                    {
                     *++(out)=*++(from);
                     *++(out)=*++(from);
                     *++(out)=*++(from);
                     len-=3;
                    }
                  //---
                  if(len)
                    {
                     *++(out)=*++(from);
                     if(len>1) *++(out)=*++(from);
                    }
                 }
               else
                 {
                  from=out-dist;
                  do
                    {
                     *++(out)=*++(from);
                     *++(out)=*++(from);
                     *++(out)=*++(from);
                     len-=3;
                    }
                  while(len>2);
                  //---
                  if(len)
                    {
                     *++(out)=*++(from);
                     if(len>1) *++(out)=*++(from);
                    }
                 }
              }
            else
               if((op&64)==0)
                 {
                  thisblock=dcode[thisblock.val+(hold&((1U<<op)-1))];
                  goto dodist;
                 }
               else
                 {
                  state->mode=BAD;
                  break;
                 }
           }
         else
            if((op&64)==0)
              {
               thisblock=lcode[thisblock.val+(hold &((1U<<op)-1))];
               goto dolen;
              }
            else
               if(op&32)
                 {
                  state->mode=TYPE;
                  break;
                 }
               else
                 {
                  state->mode=BAD;
                  break;
                 }
     }
   while(in<last && out<end);
//---
   len            =bits>>3;
   in            -=len;
   bits          -=len<<3;
   hold          &=(1U<<bits)-1;
   strm->next_in  =(char*)(in + 1);
   strm->next_out =(char*)(out + 1);
   strm->avail_in =(unsigned)(in<last ? 5+(last-in) : 5-(in-last));
   strm->avail_out=(unsigned)(out<end ? 257+(end-out) : 257-(out-end));
   state->hold    =hold;
   state->bits    =bits;
//---
   return;
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
int SZlib::inflate(z_stream *strm,int flush)
  {
   inflate_state  *state;
   BYTE           *next,*put,*from;
   unsigned        have,left,bits,in,out,copy,len;
   uint32_t        hold;
   code            thisblock,last;
   int32_t             ret;
   BYTE            hbuf[4];
   static const WORD order[19]={16, 17, 18, 0, 8, 7, 9, 6, 10, 5, 11, 4, 12, 3, 13, 2, 14, 1, 15};
//---
   if(strm==NULL || strm->state==NULL || strm->next_out==0 || (strm->next_in==0 && strm->avail_in!=0))
      return(-2);
//---
   state=(inflate_state*)strm->state;
   if(state->mode==TYPE) state->mode=TYPEDO;
//---
   put =(BYTE*)strm->next_out;
   left=strm->avail_out;
   next=(BYTE*)strm->next_in;
   have=strm->avail_in;
   hold=state->hold;
   bits=state->bits;
   in  =have;
   out =left;
   ret =0;
//---
   for(;;)
      switch(state->mode)
        {
         case HEAD:
            if(state->wrap==0)
              {
               state->mode=TYPEDO;
               break;
              }
            //---
            while(bits<16)
              {
               if(have==0) goto inf_leave;
               //---
               have--; hold+=(uint32_t)(*next++)<<bits; bits+=8;
              }
            //---
            if((state->wrap&2) && hold==0x8b1f)
              {
               state->check=crc32(0L,0,0);
               hbuf[0]=(BYTE)(hold);
               hbuf[1]=(BYTE)(hold>>8);
               state->check=crc32(state->check, hbuf, 2);
               hold=bits=0;
               state->mode=FLAGS;
               break;
              }
            //---
            state->flags=0;
            //---
            if(state->head!=0) state->head->done=-1;
            //---
            if(!(state->wrap & 1) || ((((unsigned)hold&((1U<<(8))-1))<<8)+(hold>>8))%31)
              {
               state->mode=BAD;
               break;
              }
            //---
            if(((unsigned)hold&((1U <<4)-1))!=8)
              {
               state->mode=BAD;
               break;
              }
            //---
            hold>>=4; bits -=(unsigned)(4); len=((unsigned)hold&((1U<<4)-1))+8;
            //---
            if(len>state->wbits)
              {
               state->mode=BAD;
               break;
              }
            //---
            state->dmax=1U<<len;
            strm->adler=state->check=adler32(0L, 0, 0);
            state->mode=hold & 0x200 ? DICTID : TYPE;
            hold=bits=0;
            break;
         case FLAGS:
            while(bits<(unsigned)16)
              {
               if(have==0) goto inf_leave;
               //---
               have--; hold+=(uint32_t)(*next++)<<bits; bits+=8;
              }
            //---
            state->flags=(int)(hold);
            //---
            if((state->flags&0xff)!=8)
              {
               state->mode=BAD;
               break;
              }
            //---
            if(state->flags&0xe000)
              {
               state->mode=BAD;
               break;
              }
            //---
            if(state->head!=0) state->head->text=(int)((hold>>8)&1);
            //---
            if(state->flags&0x0200)
              {
               hbuf[0]=(BYTE)(hold);
               hbuf[1]=(BYTE)((hold)>>8);
               state->check=crc32(state->check, hbuf, 2);
              }
            hold=bits=0;
            state->mode=TIME;
         case TIME:
            while(bits<32)
              {
               if(have==0) goto inf_leave;
               //---
               have--; hold+=(uint32_t)(*next++)<<bits; bits+=8;
              }
            //---
            if(state->head!=0) state->head->time=hold;
            //---
            if(state->flags & 0x0200)
              {
               hbuf[0]=(BYTE)(hold);
               hbuf[1]=(BYTE)((hold)>>8);
               hbuf[2]=(BYTE)((hold)>>16);
               hbuf[3]=(BYTE)((hold)>>24);
               state->check=crc32(state->check,hbuf,4);
              }
            //---
            hold=bits=0;
            state->mode=OS;
         case OS:
            while(bits<(unsigned)16)
              {
               if(have==0) goto inf_leave;
               //---
               have--; hold+=(uint32_t)(*next++)<<bits; bits+=8;
              }
            //---
            if(state->head!=0)
              {
               state->head->xflags=(int)(hold&0xff);
               state->head->os=(int)(hold>>8);
              }
            //---
            if(state->flags&0x0200)
              {
               hbuf[0]=(BYTE)(hold);
               hbuf[1]=(BYTE)((hold)>>8);
               state->check=crc32(state->check, hbuf, 2);
              }
            //---
            hold=bits=0;
            state->mode=EXLEN;
         case EXLEN:
            if(state->flags&0x0400)
              {
               while(bits<(unsigned)16)
                 {
                  if(have==0) goto inf_leave;
                  //---
                  have--; hold+=(uint32_t)(*next++)<<bits; bits+=8;
                 }
               //---
               state->length=(unsigned)(hold);
               //---
               if(state->head!=0) state->head->extra_len=(unsigned)hold;
               //---
               if(state->flags&0x0200)
                 {
                  hbuf[0]=(BYTE)(hold);
                  hbuf[1]=(BYTE)((hold)>>8);
                  state->check=crc32(state->check, hbuf, 2);
                 }
               //---
               hold=bits=0;
              }
            else
               if(state->head!=0) state->head->extra=0;
            //---
            state->mode=EXTRA;
         case EXTRA:
            if(state->flags&0x0400)
              {
               copy=state->length;
               if(copy>have) copy=have;
               //---
               if(copy)
                 {
                  if(state->head!=0 && state->head->extra!=0)
                    {
                     len=state->head->extra_len-state->length;
                     memcpy(state->head->extra+len,next,len+copy>state->head->extra_max ? state->head->extra_max - len : copy);
                    }
                  //---
                  if(state->flags&0x0200) state->check=crc32(state->check, next, copy);
                  //---
                  have-=copy;
                  next+=copy;
                  state->length-=copy;
                 }
               //---
               if(state->length) goto inf_leave;
              }
            state->length=0;
            state->mode=NAME;
         case NAME:
            if(state->flags&0x0800)
              {
               if(have==0) goto inf_leave;
               copy=0;
               do
                 {
                  len=(unsigned)(next[copy++]);
                  if(state->head!=0 && state->head->name!=0 && state->length<state->head->name_max)
                     state->head->name[state->length++]=(char)len;
                 }
               while(len && copy<have);
               //---
               if(state->flags&0x0200) state->check=crc32(state->check, next, copy);
               //---
               have-=copy;
               next+=copy;
               //---
               if(len) goto inf_leave;
              }
            else
               if(state->head!=0) state->head->name=0;
            //---
            state->length=0;
            state->mode=COMMENT;
         case COMMENT:
            if(state->flags&0x1000)
              {
               if(have==0) goto inf_leave;
               copy=0;
               do
                 {
                  len=(unsigned)(next[copy++]);
                  if(state->head!=0 && state->head->comment!=0 && state->length<state->head->comm_max)
                     state->head->comment[state->length++]=(char)len;
                 }
               while(len && copy<have);
               //---
               if(state->flags&0x0200) state->check=crc32(state->check,next,copy);
               have-=copy;
               next+=copy;
               if(len) goto inf_leave;
              }
            else
               if(state->head!=0) state->head->comment=0;
            //---
            state->mode=HCRC;
         case HCRC:
            if(state->flags&0x0200)
              {
               while(bits<16)
                 {
                  if(have==0) goto inf_leave;
                  //---
                  have--; hold+=(uint32_t)(*next++)<<bits; bits+=8;
                 }
               //---
               if(hold!=(state->check&0xffff))
                 {
                  state->mode=BAD;
                  break;
                 }
               //---
               hold=bits=0;
              }
            //---
            if(state->head!=0)
              {
               state->head->hcrc=(int)((state->flags>>9)&1);
               state->head->done=1;
              }
            //---
            strm->adler=state->check=crc32(0L,0,0);
            state->mode=TYPE;
            break;
         case DICTID:
            while(bits<(unsigned)32)
              {
               if(have==0) goto inf_leave;
               //---
               have--; hold+=(uint32_t)(*next++)<<bits; bits+=8;
              }
            //---
            strm->adler=state->check=((((hold)>>24)&0xff)+(((hold)>>8)&0xff00)+(((hold)&0xff00)<<8)+(((hold)&0xff)<<24));
            hold=bits=0;
            state->mode=DICT;
         case DICT:
            if(state->havedict==0)
              {
               strm->next_out=(char*)put;
               strm->avail_out=left;
               strm->next_in=(char*)next;
               strm->avail_in=have;
               state->hold=hold;
               state->bits=bits;
               return(2);
              }
            //---
            strm->adler=state->check=adler32(0L,0,0);
            state->mode=TYPE;
         case TYPE:
            if(flush==5) goto inf_leave;
         case TYPEDO:
            if(state->last)
              {
               hold>>=bits&7;
               bits -=bits&7;
               state->mode=CHECK;
               break;
              }
            //---
            while(bits<(unsigned)3)
              {
               if(have==0) goto inf_leave;
               //---
               have--; hold+=(uint32_t)(*next++)<<bits; bits+=8;
              }
            //---
            state->last=((unsigned)hold&((1U<<1)-1));
            hold>>=1;
            bits -=(unsigned)1;
            //---
            switch(((unsigned)hold & ((1U << (2)) - 1)))
              {
               case 0: state->mode=STORED;                  break;
               case 1: fixedtables(state); state->mode=LEN; break;
               case 2: state->mode=TABLE;                   break;
               case 3: state->mode=BAD;                     break;
              }
            //---
            hold>>=2;
            bits -=(unsigned)2;
            break;
         case STORED:
            hold>>=bits&7;
            bits -=bits&7;
            //---
            while(bits<32)
              {
               if(have==0) goto inf_leave;
               //---
               have--; hold+=(uint32_t)(*next++)<<bits; bits+=8;
              }
            //---
            if((hold&0xffff)!=((hold>>16)^0xffff))
              {
               state->mode=BAD;
               break;
              }
            //---
            state->length=(unsigned)hold&0xffff;
            hold=bits=0;
            state->mode=COPY;
         case COPY:
            copy=state->length;
            if(copy)
              {
               if(copy>have) copy=have;
               if(copy>left) copy=left;
               //---
               if(copy==0) goto inf_leave;
               //---
               memcpy(put,next,copy);
               have-=copy;
               next+=copy;
               left-=copy;
               put +=copy;
               state->length-=copy;
               break;
              }
            //---
            state->mode=TYPE;
            break;
         case TABLE:
            while(bits<14)
              {
               if(have==0) goto inf_leave;
               //---
               have--; hold+=(uint32_t)(*next++)<<bits; bits+=8;
              }
            //---
            state->nlen =((unsigned)hold&((1U<<5)-1))+257; hold>>=5; bits-=(unsigned)5;
            state->ndist=((unsigned)hold&((1U<<5)-1))+1;   hold>>=5; bits-=(unsigned)5;
            state->ncode=((unsigned)hold&((1U<<4)-1))+4;   hold>>=4; bits-=(unsigned)4;
            //---
            if(state->nlen>286 || state->ndist>30)
              {
               state->mode=BAD;
               break;
              }
            //---
            state->have=0;
            state->mode=LENLENS;
         case LENLENS:
            while(state->have<state->ncode)
              {
               while(bits<3)
                 {
                  if(have==0) goto inf_leave;
                  //---
                  have--; hold+=(uint32_t)(*next++)<<bits; bits+=8;
                 }
               state->lens[order[state->have++]]=(WORD)((unsigned)hold&((1U<<3)-1));
               hold>>=3; bits-=3;
              }
            //---
            while(state->have<19) state->lens[order[state->have++]]=0;
            //---
            state->next=state->codes;
            state->lencode=(const code *)(state->next);
            state->lenbits=7;
            //---
            ret=inflate_table(CODES,state->lens,19,&state->next,&state->lenbits,state->work);
            if(ret)
              {
               state->mode=BAD;
               break;
              }
            //---
            state->have=0;
            state->mode=CODELENS;
         case CODELENS:
            while(state->have<state->nlen + state->ndist)
              {
               for(;;)
                 {
                  thisblock=state->lencode[((unsigned)hold&((1U<<(state->lenbits))-1))];
                  if((unsigned)(thisblock.bits)<=bits) break;
                  if(have==0) goto inf_leave;
                  //---
                  have--; hold+=(uint32_t)(*next++)<<bits; bits+=8;
                 }
               //---
               if(thisblock.val<16)
                 {
                  while(bits<(unsigned)(thisblock.bits))
                    {
                     if(have==0) goto inf_leave;
                     //---
                     have--; hold+=(uint32_t)(*next++)<<bits; bits+=8;
                    }
                  //---
                  hold>>=(thisblock.bits);
                  bits -=(unsigned)(thisblock.bits);
                  state->lens[state->have++]=thisblock.val;
                 }
               else
                 {
                  if(thisblock.val==16)
                    {
                     while(bits<(unsigned)(thisblock.bits+2))
                       {
                        if(have==0) goto inf_leave;
                        //---
                        have--; hold+=(uint32_t)(*next++)<<bits; bits+=8;
                       }
                     //---
                     hold>>= (thisblock.bits);
                     bits -=(unsigned)(thisblock.bits);
                     if(state->have==0)
                       {
                        state->mode=BAD;
                        break;
                       }
                     //---
                     len=state->lens[state->have-1];
                     copy=3+((unsigned)hold&((1U<<2)-1));
                     //---
                     hold>>=2;
                     bits -=(unsigned)2;
                    }
                  else
                     if(thisblock.val==17)
                       {
                        //---
                        while(bits<(unsigned)(thisblock.bits+3))
                          {
                           //---
                           if(have==0) goto inf_leave;
                           //---
                           have--; hold+=(uint32_t)(*next++)<<bits; bits+=8;
                          }
                        //---
                        hold>>=(thisblock.bits);
                        bits -=(unsigned)(thisblock.bits);
                        //---
                        len=0;
                        copy=3+((unsigned)hold&((1U<<3)-1));
                        hold>>=3;
                        bits -=(unsigned)3;
                       }
                     else
                       {
                        while(bits<(unsigned)(thisblock.bits+7))
                          {
                           if(have==0) goto inf_leave;
                           //---
                           have--; hold+=(uint32_t)(*next++)<<bits; bits+=8;
                          }
                        //---
                        hold>>=(thisblock.bits); bits-=(unsigned)(thisblock.bits);
                        //---
                        len=0;
                        copy=11+((unsigned)hold&((1U<<7)-1));
                        hold>>=7; bits-=(unsigned)7;
                       }
                  //---
                  if(state->have + copy>state->nlen + state->ndist)
                    {
                     state->mode=BAD;
                     break;
                    }
                  //---
                  while(copy--) state->lens[state->have++]=(WORD)len;
                 }
              }
            //---
            if(state->mode==BAD) break;
            //---
            state->next=state->codes;
            state->lencode=(code const*)state->next;
            state->lenbits=9;
            ret=inflate_table(LENS,state->lens,state->nlen,&(state->next),&(state->lenbits),state->work);
            //---
            if(ret)
              {
               state->mode=BAD;
               break;
              }
            //---
            state->distcode=(code const*)state->next;
            state->distbits=6;
            //---
            ret=inflate_table(DISTS,state->lens+state->nlen,state->ndist,&(state->next),&(state->distbits),state->work);
            if(ret)
              {
               state->mode=BAD;
               break;
              }
            //---
            state->mode=LEN;
         case LEN:
            if(have>=6 && left>=258)
              {
               strm->next_out=(char*)put;
               strm->avail_out=left;
               strm->next_in=(char*)next;
               strm->avail_in=have;
               state->hold=hold;
               state->bits=bits;
               inflate_fast(strm, out);
               put=(BYTE*)strm->next_out;
               left=strm->avail_out;
               next=(BYTE*)strm->next_in;
               have=strm->avail_in;
               hold=state->hold;
               bits=state->bits;
               break;
              }
            //---
            for(;;)
              {
               //---
               thisblock=state->lencode[((unsigned)hold&((1U <<(state->lenbits))-1))];
               //---
               if((unsigned)(thisblock.bits)<=bits)
                  break;
               //---
               if(have==0)
                  goto inf_leave;
               //---
               have--;
               hold+=(uint32_t)(*next++)<<bits;
               bits+=8;
              }
            //---
            if(thisblock.op&&(thisblock.op&0xf0)==0)
              {
               last=thisblock;
               for(;;)
                 {
                  //---
                  thisblock=state->lencode[last.val+(((unsigned)hold&((1U<<(last.bits+last.op))-1))>>last.bits)];
                  //---
                  if((unsigned)(last.bits+thisblock.bits)<=bits)
                     break;
                  //---
                  if(have==0)
                     goto inf_leave;
                  //---
                  have--; hold+=(uint32_t)(*next++)<<bits; bits+=8;
                 }
               //---
               hold>>=(last.bits);
               bits-=(unsigned)(last.bits);
              }
            //---
            hold>>=(thisblock.bits); bits-=(unsigned)(thisblock.bits);
            //---
            state->length=(unsigned)thisblock.val;
            if((int)(thisblock.op)==0)
              {
               state->mode=LIT;
               break;
              }
            //---
            if(thisblock.op&32)
              {
               state->mode=TYPE;
               break;
              }
            //---
            if(thisblock.op&64)
              {
               state->mode=BAD;
               break;
              }
            //---
            state->extra=(unsigned)(thisblock.op) & 15;
            state->mode =LENEXT;
         case LENEXT:
            if(state->extra)
              {
               //---
               while(bits<(unsigned)state->extra)
                 {
                  if(have==0) goto inf_leave;
                  //---
                  have--; hold+=(uint32_t)(*next++)<<bits; bits+=8;
                 }
               //---
               state->length+=((unsigned)hold&((1U<<(state->extra))-1));
               //---
               hold>>=(state->extra); bits -=(unsigned)(state->extra);
              }
            //---
            state->mode=DIST;
         case DIST:
            for(;;)
              {
               thisblock=state->distcode[((unsigned)hold & ((1U << (state->distbits)) - 1))];
               if((unsigned)(thisblock.bits)<=bits) break;
               if(have==0) goto inf_leave;
               //---
               have--; hold+=(uint32_t)(*next++)<<bits; bits+=8;
              }
            //---
            if((thisblock.op & 0xf0)==0)
              {
               last=thisblock;
               //---
               for(;;)
                 {
                  //---
                  thisblock=state->distcode[last.val+(((unsigned)hold&((1U<<(last.bits+last.op))-1))>>last.bits)];
                  if((unsigned)(last.bits+thisblock.bits)<=bits) break;
                  //---
                  if(have==0) goto inf_leave;
                  //---
                  have--; hold+=(uint32_t)(*next++)<<bits; bits+=8;
                 }
               //---
               hold>>=(last.bits); bits-=(unsigned)(last.bits);
              }
            //---
            hold>>=(thisblock.bits); bits-=(unsigned)(thisblock.bits);
            //---
            if(thisblock.op&64)
              {
               state->mode=BAD;
               break;
              }
            //---
            state->offset=(unsigned)thisblock.val;
            state->extra =(unsigned)(thisblock.op)&15;
            state->mode  =DISTEXT;
         case DISTEXT:
            if(state->extra)
              {
               while(bits<(unsigned)state->extra)
                 {
                  if(have==0) goto inf_leave;
                  //---
                  have--; hold+=(uint32_t)(*next++)<<bits; bits+=8;
                 }
               //---
               state->offset+=((unsigned)hold&((1U<<(state->extra))-1));
               //---
               hold>>=(state->extra);  bits-=(unsigned)(state->extra);
              }
            //---
            if(state->offset>state->whave+out-left)
              {
               state->mode=BAD;
               break;
              }
            //---
            state->mode=MATCH;
         case MATCH:
            if(left==0)
               goto inf_leave;
            //---
            copy=out-left;
            //---
            if(state->offset>copy)
              {
               copy=state->offset-copy;
               if(copy>state->write)
                 {
                  copy-=state->write;
                  from=state->window + (state->wsize - copy);
                 }
               else from=state->window + (state->write-copy);
               //---
               if(copy>state->length) copy=state->length;
              }
            else
              {
               from=put-state->offset;
               copy=state->length;
              }
            //---
            if(copy>left) copy=left;
            //---
            left-=copy; state->length-=copy;
            //---
         do { *put++=*from++; } while(--copy);
         //---
         if(state->length==0)
            state->mode=LEN;
         break;

         case LIT:
            if(left==0) goto inf_leave;
            *put++=(BYTE)(state->length);
            left--;
            state->mode=LEN;
            break;
         case CHECK:
            if(state->wrap)
              {
               while(bits<32)
                 {
                  if(have==0) goto inf_leave;
                  //---
                  have--; hold+=(uint32_t)(*next++)<<bits; bits+=8;
                 }
               //---
               out            -=left;
               strm->total_out+=out;
               state->total   +=out;
               //---
               if(out)
                  strm->adler=state->check=(state->flags?
                     crc32(state->check,put-out,out) : adler32(state->check,put-out,out));
               out=left;
               //---
               if((state->flags ?
                  hold : ((((hold)>>24)&0xff)+(((hold)>>8)&0xff00)+(((hold)&0xff00)<<8)+(((hold)&0xff)<<24)))!=state->check)
                 {
                  state->mode=BAD;
                  break;
                 }
               //---
               hold=0; bits=0;
              }
            state->mode=LENGTH;

         case LENGTH:
            if(state->wrap && state->flags)
              {
               //---
               while(bits<(unsigned)32)
                 {
                  if(have==0)
                     goto inf_leave;
                  //---
                  have--;
                  hold+=(uint32_t)(*next++)<<bits;
                  bits+=8;
                 }
               //---
               if(hold!=(state->total&0xffffffffUL))
                 {
                  state->mode=BAD;
                  break;
                 }
               //---
               hold=0; bits=0;
              }
            state->mode=DONE;

         case DONE:
            ret=1;
            goto inf_leave;

         case BAD:
            ret=-3;
            goto inf_leave;

         case MEM:
            return(-4);

         case SYNC:
         default:
            return(-2);
        }
//---
   inf_leave:
   strm->next_out =(char*)put;
   strm->avail_out=left;
   strm->next_in  =(char*)next;
   strm->avail_in =have;
   state->hold    =hold;
   state->bits    =bits;
   if(state->wsize || (state->mode<CHECK && out!=strm->avail_out))
      if(updatewindow(strm,out))
        {
         state->mode=MEM;
         return(-4);
        }
//---
   in-=strm->avail_in;
   out-=strm->avail_out;
   strm->total_in+=in;
   strm->total_out+=out;
   state->total+=out;
   if(state->wrap && out)
      strm->adler=state->check=(state->flags ? crc32(state->check,(BYTE*)(strm->next_out-out),out) : adler32(state->check,(BYTE*)(strm->next_out-out),out));
//---
   strm->data_type=state->bits+(state->last?64:0)+(state->mode==TYPE ? 128 : 0);
//---
   if(((in==0 && out==0) || flush==4) && ret==0)
      ret=-5;
//---
   return(ret);
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
int SZlib::inflateEnd(z_stream *strm)
  {
   inflate_state *state;
//---
   if(strm==NULL || strm->state==NULL || strm->zfree==NULL) return(-2);
//---
   state=(inflate_state*)strm->state;
//---
   if(state->window!=NULL) (*(strm->zfree))(state->window);
//---
   (*(strm->zfree))(strm->state);
   strm->state=0;
   return(0);
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
int SZlib::inflateReset(z_stream *strm)
  {
   inflate_state *state;
//---
   if(strm==0 || strm->state==0) return(-2);
//---
   state=(inflate_state*)strm->state;
//---
   strm->total_in =strm->total_out=state->total=0;
   strm->adler    =1;
   state->mode    =HEAD;
   state->last    =0;
   state->havedict=0;
   state->dmax    =32768U;
   state->head    =0;
   state->wsize   =0;
   state->whave   =0;
   state->write   =0;
   state->hold    =0;
   state->bits    =0;
   state->lencode =state->distcode=state->next=state->codes;
//---
   return(0);
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
void SZlib::fixedtables(inflate_state *state)
  {
   static const code lenfix[512]=
     {
        {96,7,0},{0,8,80},{0,8,16},{20,8,115},{18,7,31},{0,8,112},{0,8,48},{0,9,192},{16,7,10},{0,8,96},{0,8,32},{0,9,160},{0,8,0},{0,8,128},
      {0,8,64},{0,9,224},{16,7,6},{0,8,88},{0,8,24},{0,9,144},{19,7,59},{0,8,120},{0,8,56},{0,9,208},{17,7,17},{0,8,104},{0,8,40},{0,9,176},
      {0,8,8},{0,8,136},{0,8,72},{0,9,240},{16,7,4},{0,8,84},{0,8,20},{21,8,227},{19,7,43},{0,8,116},{0,8,52},{0,9,200},{17,7,13},{0,8,100},
      {0,8,36},{0,9,168},{0,8,4},{0,8,132},{0,8,68},{0,9,232},{16,7,8},{0,8,92},{0,8,28},{0,9,152},{20,7,83},{0,8,124},{0,8,60},{0,9,216},
      {18,7,23},{0,8,108},{0,8,44},{0,9,184},{0,8,12},{0,8,140},{0,8,76},{0,9,248},{16,7,3},{0,8,82},{0,8,18},{21,8,163},{19,7,35},{0,8,114},
      {0,8,50},{0,9,196},{17,7,11},{0,8,98},{0,8,34},{0,9,164},{0,8,2},{0,8,130},{0,8,66},{0,9,228},{16,7,7},{0,8,90},{0,8,26},{0,9,148},
      {20,7,67},{0,8,122},{0,8,58},{0,9,212},{18,7,19},{0,8,106},{0,8,42},{0,9,180},{0,8,10},{0,8,138},{0,8,74},{0,9,244},{16,7,5},{0,8,86},
      {0,8,22},{64,8,0},{19,7,51},{0,8,118},{0,8,54},{0,9,204},{17,7,15},{0,8,102},{0,8,38},{0,9,172},{0,8,6},{0,8,134},{0,8,70},{0,9,236},
      {16,7,9},{0,8,94},{0,8,30},{0,9,156},{20,7,99},{0,8,126},{0,8,62},{0,9,220},{18,7,27},{0,8,110},{0,8,46},{0,9,188},{0,8,14},{0,8,142},
      {0,8,78},{0,9,252},{96,7,0},{0,8,81},{0,8,17},{21,8,131},{18,7,31},{0,8,113},{0,8,49},{0,9,194},{16,7,10},{0,8,97},{0,8,33},{0,9,162},
      {0,8,1},{0,8,129},{0,8,65},{0,9,226},{16,7,6},{0,8,89},{0,8,25},{0,9,146},{19,7,59},{0,8,121},{0,8,57},{0,9,210},{17,7,17},{0,8,105},
      {0,8,41},{0,9,178},{0,8,9},{0,8,137},{0,8,73},{0,9,242},{16,7,4},{0,8,85},{0,8,21},{16,8,258},{19,7,43},{0,8,117},{0,8,53},{0,9,202},
      {17,7,13},{0,8,101},{0,8,37},{0,9,170},{0,8,5},{0,8,133},{0,8,69},{0,9,234},{16,7,8},{0,8,93},{0,8,29},{0,9,154},{20,7,83},{0,8,125},
      {0,8,61},{0,9,218},{18,7,23},{0,8,109},{0,8,45},{0,9,186},{0,8,13},{0,8,141},{0,8,77},{0,9,250},{16,7,3},{0,8,83},{0,8,19},{21,8,195},
      {19,7,35},{0,8,115},{0,8,51},{0,9,198},{17,7,11},{0,8,99},{0,8,35},{0,9,166},{0,8,3},{0,8,131},{0,8,67},{0,9,230},{16,7,7},{0,8,91},
      {0,8,27},{0,9,150},{20,7,67},{0,8,123},{0,8,59},{0,9,214},{18,7,19},{0,8,107},{0,8,43},{0,9,182},{0,8,11},{0,8,139},{0,8,75},{0,9,246},
      {16,7,5},{0,8,87},{0,8,23},{64,8,0},{19,7,51},{0,8,119},{0,8,55},{0,9,206},{17,7,15},{0,8,103},{0,8,39},{0,9,174},{0,8,7},{0,8,135},
      {0,8,71},{0,9,238},{16,7,9},{0,8,95},{0,8,31},{0,9,158},{20,7,99},{0,8,127},{0,8,63},{0,9,222},{18,7,27},{0,8,111},{0,8,47},{0,9,190},
      {0,8,15},{0,8,143},{0,8,79},{0,9,254},{96,7,0},{0,8,80},{0,8,16},{20,8,115},{18,7,31},{0,8,112},{0,8,48},{0,9,193},{16,7,10},{0,8,96},
      {0,8,32},{0,9,161},{0,8,0},{0,8,128},{0,8,64},{0,9,225},{16,7,6},{0,8,88},{0,8,24},{0,9,145},{19,7,59},{0,8,120},{0,8,56},{0,9,209},
      {17,7,17},{0,8,104},{0,8,40},{0,9,177},{0,8,8},{0,8,136},{0,8,72},{0,9,241},{16,7,4},{0,8,84},{0,8,20},{21,8,227},{19,7,43},{0,8,116},
      {0,8,52},{0,9,201},{17,7,13},{0,8,100},{0,8,36},{0,9,169},{0,8,4},{0,8,132},{0,8,68},{0,9,233},{16,7,8},{0,8,92},{0,8,28},{0,9,153},
      {20,7,83},{0,8,124},{0,8,60},{0,9,217},{18,7,23},{0,8,108},{0,8,44},{0,9,185},{0,8,12},{0,8,140},{0,8,76},{0,9,249},{16,7,3},{0,8,82},
      {0,8,18},{21,8,163},{19,7,35},{0,8,114},{0,8,50},{0,9,197},{17,7,11},{0,8,98},{0,8,34},{0,9,165},{0,8,2},{0,8,130},{0,8,66},{0,9,229},
      {16,7,7},{0,8,90},{0,8,26},{0,9,149},{20,7,67},{0,8,122},{0,8,58},{0,9,213},{18,7,19},{0,8,106},{0,8,42},{0,9,181},{0,8,10},{0,8,138},
      {0,8,74},{0,9,245},{16,7,5},{0,8,86},{0,8,22},{64,8,0},{19,7,51},{0,8,118},{0,8,54},{0,9,205},{17,7,15},{0,8,102},{0,8,38},{0,9,173},
      {0,8,6},{0,8,134},{0,8,70},{0,9,237},{16,7,9},{0,8,94},{0,8,30},{0,9,157},{20,7,99},{0,8,126},{0,8,62},{0,9,221},{18,7,27},{0,8,110},
      {0,8,46},{0,9,189},{0,8,14},{0,8,142},{0,8,78},{0,9,253},{96,7,0},{0,8,81},{0,8,17},{21,8,131},{18,7,31},{0,8,113},{0,8,49},{0,9,195},
      {16,7,10},{0,8,97},{0,8,33},{0,9,163},{0,8,1},{0,8,129},{0,8,65},{0,9,227},{16,7,6},{0,8,89},{0,8,25},{0,9,147},{19,7,59},{0,8,121},
      {0,8,57},{0,9,211},{17,7,17},{0,8,105},{0,8,41},{0,9,179},{0,8,9},{0,8,137},{0,8,73},{0,9,243},{16,7,4},{0,8,85},{0,8,21},{16,8,258},
      {19,7,43},{0,8,117},{0,8,53},{0,9,203},{17,7,13},{0,8,101},{0,8,37},{0,9,171},{0,8,5},{0,8,133},{0,8,69},{0,9,235},{16,7,8},{0,8,93},
      {0,8,29},{0,9,155},{20,7,83},{0,8,125},{0,8,61},{0,9,219},{18,7,23},{0,8,109},{0,8,45},{0,9,187},{0,8,13},{0,8,141},{0,8,77},{0,9,251},
      {16,7,3},{0,8,83},{0,8,19},{21,8,195},{19,7,35},{0,8,115},{0,8,51},{0,9,199},{17,7,11},{0,8,99},{0,8,35},{0,9,167},{0,8,3},{0,8,131},
      {0,8,67},{0,9,231},{16,7,7},{0,8,91},{0,8,27},{0,9,151},{20,7,67},{0,8,123},{0,8,59},{0,9,215},{18,7,19},{0,8,107},{0,8,43},{0,9,183},
      {0,8,11},{0,8,139},{0,8,75},{0,9,247},{16,7,5},{0,8,87},{0,8,23},{64,8,0},{19,7,51},{0,8,119},{0,8,55},{0,9,207},{17,7,15},{0,8,103},
      {0,8,39},{0,9,175},{0,8,7},{0,8,135},{0,8,71},{0,9,239},{16,7,9},{0,8,95},{0,8,31},{0,9,159},{20,7,99},{0,8,127},{0,8,63},{0,9,223},
      {18,7,27},{0,8,111},{0,8,47},{0,9,191},{0,8,15},{0,8,143},{0,8,79},{0,9,255}
     };
   static const code distfix[32]=
     {
        {16,5,1},{23,5,257},{19,5,17},{27,5,4097},{17,5,5},{25,5,1025},{21,5,65},{29,5,16385},{16,5,3},{24,5,513},{20,5,33},{28,5,8193},
      {18,5,9},{26,5,2049},{22,5,129},{64,5,0},{16,5,2},{23,5,385},{19,5,25},{27,5,6145},{17,5,7},{25,5,1537},{21,5,97},{29,5,24577},
      {16,5,4},{24,5,769},{20,5,49},{28,5,12289},{18,5,13},{26,5,3073},{22,5,193},{64,5,0}
     };
//---
   state->lencode =lenfix;
   state->lenbits =9;
   state->distcode=distfix;
   state->distbits=5;
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
int SZlib::inflate_table(codetype type,const WORD *lens,unsigned codes,code **table,unsigned *bits,WORD *work)
  {
   unsigned    len,sym,min,max,root,curr,drop,used,huff,incr,fill,low,mask;
   int32_t         left,end;
   code        thisblock,*next;
   const WORD *base;
   const WORD *extra;
   WORD        count[16],offs[16];
//---
   static const WORD lbase[31]={ 3, 4, 5, 6, 7, 8, 9, 10, 11, 13, 15, 17, 19, 23, 27, 31, 35, 43, 51, 59, 67, 83, 99, 115, 131, 163, 195, 227, 258, 0, 0 };
   static const WORD lext[31] ={ 16, 16, 16, 16, 16, 16, 16, 16, 17, 17, 17, 17, 18, 18, 18, 18, 19, 19, 19, 19, 20, 20, 20, 20, 21, 21, 21, 21, 16, 201, 196 };
   static const WORD dbase[32]={ 1, 2, 3, 4, 5, 7, 9, 13, 17, 25, 33, 49, 65, 97, 129, 193, 257, 385, 513, 769, 1025, 1537, 2049, 3073, 4097, 6145, 8193, 12289, 16385, 24577, 0, 0 };
   static const WORD dext[32] ={ 16, 16, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 23, 24, 24, 25, 25, 26, 26, 27, 27, 28, 28, 29, 29, 64, 64 };
//---
   for(len=0;len<=15;len++) count[len]=0;
//---
   for(sym=0;sym<codes;sym++) count[lens[sym]]++;
   root=*bits;
//---
   for(max=15; max>=1; max--)
      if(count[max]!=0) break;
   if(root>max) root=max;
//---
   if(max==0)
     {
      thisblock.op  =64;
      thisblock.bits=1;
      thisblock.val =0;
      *(*table)++=thisblock;
      *(*table)++=thisblock;
      *bits=1;
      return(0);
     }
//---
   for(min=1; min<=15; min++)
      if(count[min]!=0) break;
//---
   if(root<min) root=min;
//---
   left=1;
//---
   for(len=1;len<=15;len++)
     {
      left<<=1;
      left-=count[len];
      if(left<0) return(-1);
     }
//---
   if(left>0 && (type==CODES || max!=1)) return(-1);
//---
   offs[1]=0;
   for(len=1;len<15;len++) offs[len+1]=offs[len]+count[len];
//---
   for(sym=0;sym<codes;sym++)
      if(lens[sym]!=0) work[offs[lens[sym]]++]=(WORD)sym;
//---
   switch(type)
     {
      case CODES:
         base=extra=work;
         end =19;
         break;
      case LENS:
         base  =lbase;
         base -=257;
         extra =lext;
         extra-=257;
         end   =256;
         break;
      default:
         base =dbase;
         extra=dext;
         end  =-1;
     }
//---
   huff=0;
   sym =0;
   len =min;
   next=*table;
   curr=root;
   drop=0;
   low =(unsigned)(-1);
   used=1U<<root;
   mask=used-1;
//---
   if(type==LENS && used>=2048-592) return(1);
//---
   for(;;)
     {
      thisblock.bits=(BYTE)(len-drop);
      //---
      if((int)(work[sym])<end)
        {
         thisblock.op=(BYTE)0;
         thisblock.val=work[sym];
        }
      else
         if((int)(work[sym])>end)
           {
            thisblock.op=(BYTE)(extra[work[sym]]);
            thisblock.val=base[work[sym]];
           }
         else
           {
            thisblock.op=(BYTE)(32+64);
            thisblock.val=0;
           }
      //---
      incr=1U<<(len-drop);
      fill=1U<<curr;
      min=fill;
      //---
      do
        {
         fill-=incr;
         next[(huff>>drop)+fill]=thisblock;
        }
      while(fill!=0);
      //---
      incr=1U<<(len-1);
      while(huff&incr) incr>>=1;
      //---
      if(incr!=0)
        {
         huff&=incr-1;
         huff+=incr;
        }
      else huff=0;
      //---
      sym++;
      //---
      if(--(count[len])==0)
        {
         if(len==max) break;
         len=lens[work[sym]];
        }
      //---
      if(len>root&&(huff&mask)!=low)
        {
         if(drop==0) drop=root;
         next+=min;
         curr =len-drop;
         left =(int)(1<<curr);
         //---
         while(curr+drop<max)
           {
            left-=count[curr + drop];
            if(left<=0) break;
            curr++;
            left <<= 1;
           }
         //---
         used+=1U<<curr;
         if(type==LENS && used>=2048 - 592) return(1);
         //---
         low=huff & mask;
         (*table)[low].op  =(BYTE)curr;
         (*table)[low].bits=(BYTE)root;
         (*table)[low].val =(WORD)(next-*table);
        }
     }
//---
   thisblock.op  =(BYTE)64;
   thisblock.bits=(BYTE)(len-drop);
   thisblock.val =(WORD)0;
//---
   while(huff!=0)
     {
      //---
      if(drop!=0 && (huff&mask)!=low)
        {
         drop          =0;
         len           =root;
         next          =*table;
         thisblock.bits=(BYTE)len;
        }
      //---
      next[huff>>drop]=thisblock;
      //---
      incr=1U<<(len-1);
      //---
      while(huff & incr) incr>>=1;
      if(incr!=0)
        {
         huff&=incr-1;
         huff+=incr;
        }
      else huff=0;
     }
//---
   *table+=used;
   *bits=root;
//---
   return(0);
  }
//+------------------------------------------------------------------+
//|                                                                  |
//+------------------------------------------------------------------+
int SZlib::updatewindow(const z_stream *strm,unsigned out)
  {
   inflate_state *state;
   unsigned       copy,dist;
//---
   state=(inflate_state*)strm->state;
//---
   if(state->window==0)
     {
      state->window=(BYTE*)(*(strm->zalloc))(1U<<state->wbits,sizeof(BYTE));
      if(state->window==NULL) return(1);
     }
//---
   if(state->wsize==0)
     {
      state->wsize=1U<<state->wbits;
      state->write=0;
      state->whave=0;
     }
//---
   copy=out-strm->avail_out;
//---
   if(copy>=state->wsize)
     {
      memcpy(state->window,strm->next_out-state->wsize,state->wsize);
      state->write=0;
      state->whave=state->wsize;
     }
   else
     {
      dist=state->wsize-state->write;
      if(dist>copy) dist=copy;
      memcpy(state->window+state->write,strm->next_out-copy,dist);
      copy-=dist;
      if(copy)
        {
         memcpy(state->window,strm->next_out-copy,copy);
         state->write=copy;
         state->whave=state->wsize;
        }
      else
        {
         state->write+=dist;
         if(state->write==state->wsize) state->write=0;
         if(state->whave<state->wsize)  state->whave+=dist;
        }
     }
//---
   return(0);
  }
//+------------------------------------------------------------------+
