<!-- Improved compatibility of back to top link: See: https://github.com/Artemia76/Ham-ESP32-RPT/pull/73 -->
<a id="readme-top"></a>
<!--
*** Thanks for checking out the Best-README-Template. If you have a suggestion
*** that would make this better, please fork the repo and create a pull request
*** or simply open an issue with the tag "enhancement".
*** Don't forget to give the project a star!
*** Thanks again! Now go create something AMAZING! :D
-->



<!-- PROJECT SHIELDS -->
<!--
*** I'm using markdown "reference style" links for readability.
*** Reference links are enclosed in brackets [ ] instead of parentheses ( ).
*** See the bottom of this document for the declaration of the reference variables
*** for contributors-url, forks-url, etc. This is an optional, concise syntax you may use.
*** https://www.markdownguide.org/basic-syntax/#reference-style-links
-->
[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![GPL v3][license-shield]][license-url]



<!-- PROJECT LOGO -->
<br />
<div align="center">
  <a href="https://github.com/Artemia76/Ham-ESP32-RPT">
    <img src="images/logo.png" alt="Logo" width="80" height="80">
  </a>

<h3 align="center">HAM-ESP32-RPT</h3>

  <p align="center">
    An ESP32 Based repeater
    <br />
    <a href="https://github.com/Artemia76/Ham-ESP32-RPT"><strong>Explore the docs »</strong></a>
    <br />
    <br />
    <a href="https://github.com/Artemia76/Ham-ESP32-RPT">View Demo</a>
    &middot;
    <a href="https://github.com/Artemia76/Ham-ESP32-RPT/issues/new?labels=bug&template=bug-report---.md">Report Bug</a>
    &middot;
    <a href="https://github.com/Artemia76/Ham-ESP32-RPT/issues/new?labels=enhancement&template=feature-request---.md">Request Feature</a>
  </p>
</div>



<!-- TABLE OF CONTENTS -->
<details>
  <summary>Table of Contents</summary>
  <ol>
    <li>
      <a href="#about-the-project">About The Project</a>
      <ul>
        <li><a href="#built-with">Built With</a></li>
      </ul>
    </li>
    <li>
      <a href="#getting-started">Getting Started</a>
      <ul>
        <li><a href="#prerequisites">Prerequisites</a></li>
        <li><a href="#installation">Installation</a></li>
      </ul>
    </li>
    <li><a href="#usage">Usage</a></li>
    <li><a href="#roadmap">Roadmap</a></li>
    <li><a href="#contributing">Contributing</a></li>
    <li><a href="#license">License</a></li>
    <li><a href="#contact">Contact</a></li>
    <li><a href="#acknowledgments">Acknowledgments</a></li>
  </ol>
</details>



<!-- ABOUT THE PROJECT -->
## About The Project

[![Product Name Screen Shot][product-screenshot]](https://example.com)

This project aim to purpose universal HAM Radio Analog Repeater based on ESP32 MCU.

It based on :
  * ESP32-S3 DevKit Wroom board
  * PmodI2S2 ADC + DAC I2S moodule
  * INA 219 I2C Voltage measurement chip

In addition (optional) we add :
  * Audio monitoring amplifier
  * Test button
  * Red an Green LED state indication

The Repeater feature are :
  * Stream in the RX audio
  * Stream out the TX audio
  * Play start and end announce
  * Detect 1750 Hz tone to engage
  * Read RSSI value by using voltage reading through _ina219 chip
  * TOT ( time out timer) management
  * Web management through WiFi
  * 

<p align="right">(<a href="#readme-top">back to top</a>)</p>



### Built With


* [![PIO Arduino][PIO.js]][PIO-url]

KiCAD for PCB design

Audacity for wav design

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- GETTING STARTED -->
## Getting Started

This is an example of how you may give instructions on setting up your project locally.
To get a local copy up and running follow these simple example steps.

### Prerequisites

**VS Code**
This project need Microsoft [VS Code IDE](https://code.visualstudio.com)

**PIO Arduino**
Search for `pioarduino` on the [Visual Studio Code Marketplace](https://marketplace.visualstudio.com/search?term=pioarduino&target=VSCode&category=All%20categories&sortBy=Relevance) and follow the documentation [PlatformIO IDE for VSCode](http://docs.platformio.org/page/ide/vscode.html) how to install.

### Installation

_Below is an example of how you can instruct your audience on installing and setting up your app. This template doesn't rely on any external dependencies or services._

1. Clone the repo
   ```sh
   git clone https://github.com/Artemia76/Ham-ESP32-RPT.git
   ```
3. Open folder in VS Code
   
4. Copy and rename the file include/env.hpp.dist to include/env.hpp

5. Enter Wifi Credential has needed in include/env.hpp
   ```
   #define WIFI_SSID "My SSID"
   #define WIFI_PASSWORD "MyVerySecretPassword"
   ```

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- USAGE EXAMPLES -->
## Usage

Use this space to show useful examples of how a project can be used. Additional screenshots, code examples and demos work well in this space. You may also link to more resources.

_For more examples, please refer to the [Documentation](https://example.com)_

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- ROADMAP -->
## Roadmap

- [x] Audio 1750 Hz FFT Detection
- [x] Repeater Sequence
- [ ] Web Management
- [X] Permanent Memory
- [ ] PCB Design

See the [open issues](https://github.com/Artemia76/Ham-ESP32-RPT/issues) for a full list of proposed features (and known issues).

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- CONTRIBUTING -->
## Contributing

Contributions are what make the open source community such an amazing place to learn, inspire, and create. Any contributions you make are **greatly appreciated**.

If you have a suggestion that would make this better, please fork the repo and create a pull request. You can also simply open an issue with the tag "enhancement".
Don't forget to give the project a star! Thanks again!

1. Fork the Project
2. Create your Feature Branch (`git checkout -b feature/AmazingFeature`)
3. Commit your Changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the Branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

### Top contributors:

<a href="https://github.com/Artemia76/Ham-ESP32-RPT/graphs/contributors">
  <img src="https://contrib.rocks/image?repo=Artemia76/Ham-ESP32-RPT" alt="contrib.rocks image" />
</a>

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- LICENSE -->
## License

Distributed under the Unlicense License. See `LICENSE.txt` for more information.

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- CONTACT -->
## Contact

F4IKZ - [@Neophile76](https://twitter.com/artemia76) - neophile76@gmail.com

Project Link: [https://github.com/Artemia76/Ham-ESP32-RPT](https://github.com/Artemia76/Ham-ESP32-RPT)

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- ACKNOWLEDGMENTS -->
## Acknowledgments

Use this space to list resources you find helpful and would like to give credit to. I've included a few of my favorites to kick things off!

* [Choose an Open Source License](https://choosealicense.com)
* [GitHub Emoji Cheat Sheet](https://www.webpagefx.com/tools/emoji-cheat-sheet)
* [Malven's Flexbox Cheatsheet](https://flexbox.malven.co/)
* [Malven's Grid Cheatsheet](https://grid.malven.co/)
* [Img Shields](https://shields.io)
* [GitHub Pages](https://pages.github.com)
* [Font Awesome](https://fontawesome.com)
* [React Icons](https://react-icons.github.io/react-icons/search)

<p align="right">(<a href="#readme-top">back to top</a>)</p>



<!-- MARKDOWN LINKS & IMAGES -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links -->
[contributors-shield]: https://img.shields.io/github/contributors/othneildrew/Best-README-Template.svg?style=for-the-badge
[contributors-url]: https://github.com/Artemia76/Ham-ESP32-RPT/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/othneildrew/Best-README-Template.svg?style=for-the-badge
[forks-url]: https://github.com/Artemia76/Ham-ESP32-RPT/network/members
[stars-shield]: https://img.shields.io/github/stars/othneildrew/Best-README-Template.svg?style=for-the-badge
[stars-url]: https://github.com/Artemia76/Ham-ESP32-RPT/stargazers
[issues-shield]: https://img.shields.io/github/issues/othneildrew/Best-README-Template.svg?style=for-the-badge
[issues-url]: https://github.com/Artemia76/Ham-ESP32-RPT/issues
[license-shield]: https://img.shields.io/github/license/othneildrew/Best-README-Template.svg?style=for-the-badge
[license-url]: https://github.com/Artemia76/Ham-ESP32-RPT/blob/master/LICENSE.txt
[product-screenshot]: images/screenshot.jpg
[PIO.js]: https://img.shields.io/badge/pioarduino-000000?style=for-the-badge&logo=pioarduino&logoColor=white
[PIO-url]: https://github.com/pioarduino
