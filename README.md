# miscgen #

misgen is used to generate an original misc raw partition image (misc.img by default)

Parameters
* xPrio: slot priority = value between 0 and 15 (higher value is higher priority)
* xTry: number of remaining tries for this slot = value between 0 and 7
* xSuccess: boot successful for this slot = value 0 (false) or 1 (true)

Recommendation (initial image for _a/_b partitions), execute following command:
    miscgen 7 7 1 0 7 0

It is part of the STMicroelectronics delivery for Android (see the [delivery][] for more information).

[delivery]: https://wiki.st.com/stm32mpu/wiki/STM32MP15_distribution_for_Android_release_note_-_v2.0.0

## Description ##

This module version has been adapted to the boot control API v1.1 using default implementation
Please see the release notes for more details.

## Documentation ##

* The [release notes][] provide information on the release.
* The [distribution package][] provides detailed information on how to use this delivery.

[release notes]: https://wiki.st.com/stm32mpu/wiki/STM32MP15_distribution_for_Android_release_note_-_v2.0.0
[distribution package]: https://wiki.st.com/stm32mpu/wiki/STM32MP1_Distribution_Package_for_Android

## Dependencies ##

This module can't be used alone. It is part of the STMicroelectronics delivery for Android.

## Containing ##

This directory contains the sources and associated Android makefile to generate the miscgen host executable. 

## License ##

This module is distributed under the Apache License, Version 2.0 found in the [LICENSE](./LICENSE) file.
