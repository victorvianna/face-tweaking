# FaceTweaking


Scalable application for tweaking a photo of a face. Ex: Adding beard, changing eye color, making her smile

![Eye color changing](https://raw.githubusercontent.com/victorvianna/FaceTweaking/master/screenshots/eyes-screenshot.png )  
![Beard addition](https://raw.githubusercontent.com/victorvianna/FaceTweaking/master/screenshots/beard-screenshot.png )  

Keywords: Image morphing, seamless cloning, factory design pattern

## Authors

Cauim de Souza Lima  
Gabriel Fedrigo Barcik  
Victor Hugo Vianna

## Requirements

Java 1.8  
OpenCV 3.2

## Instructions

To compile, open a shell and run 
```
$ bash COMPILE.sh
```
To start the application, add the desired image file as image.jpg (jpg, not jpeg) to the project root, open a shell and run 
```
$ sudo bash RUN.sh
```
Choose which tweak you want to apply and the option (Ex: Change eye color / Blue eyes).  
Important: After the tweaked image is shown, PRESS SPACE TO CONTINUE. Do not close the image!

## Scalability: Implementing new features

To implement a new feature, it suffices to create a new folder in lib/features that contains YourClass.h/YourClass.cpp (inheriting from BaseFeature) and a 'media' directory with the images you will use during the morphing. You also need to adapt engine/main.cpp accordingly.

## Acknowledgements

https://github.com/spmallick/learnopencv

