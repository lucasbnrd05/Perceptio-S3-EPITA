# PERCEPTIO

PERCEPTIO is an Optical Character Recognition (OCR) project designed to analyze images, detect letters and words, and extract textual information.
This project utilizes various algorithms and machine learning techniques to provide an effective and accurate solution.

---
## Group Members

- Lucas
- Ayemane
- Matyas
- Michaël

---
## Project Structure

The project is organized to separate different functionalities and modules for better management and understanding.
Here is an overview of the file structure:

```
.
├── AUTHORS
├── Makefile
├── README.md
├── crop
│   ├── Makefile
│   ├── crop_image.c
│   ├── crop_image.h
│   └── main.c
├── grid_finder
│   ├── Makefile
│   ├── detection_letter.c
│   ├── detection_letter.h
│   ├── detection_word.c
│   ├── detection_word.h
│   ├── grid_finder.c
│   ├── grid_finder.h
│   ├── main.c
│   ├── main.h
│   └── temp
│       └── notempty
├── image_binarization
│   ├── Makefile
│   ├── binarization.c
│   ├── binarization.h
│   ├── image_resize.c
│   └── main.c
├── interface_main.c
├── letter_ia
│   ├── creation_dataset.c
│   ├── creation_dataset.h
│   ├── ia_letter.c
│   ├── ia_letter.h
│   └── main.c
├── logo
│   ├── logo.ico
│   └── logo.png
├── neuron
│   ├── neuron.c
│   └── neuron.h
├── nxor
│   ├── ia_proof.c
│   └── ia_proof.h
├── rapport_1.pdf
├── rapport_2.pdf
├── resolve.c
├── resolve.h
├── rotation_analysis
│   ├── Makefile
│   ├── main.c
│   ├── manual_grid_rotation.c
│   └── manual_grid_rotation.h
├── solver_src
│   ├── Makefile
│   ├── main.c
│   ├── solver.c
│   └── solver.h
├── source_file
│   ├── data_base
│   │   ├── dataset_labels.txt
│   │   ├── modelletter.txt
│   │   └── modelxor.txt
│   ├── grid_txt
│   │   ├── grid
│   │   └── grid2
│   ├── image_grid
│   │   ├── level_1_image_1.png
│   │   ├── level_1_image_2.png
│   │   ├── level_2_image_1.png
│   │   ├── level_2_image_2.png
│   │   ├── level_3_image_1.png
│   │   ├── level_3_image_2.png
│   │   ├── level_4_image_1.png
│   │   └── level_4_image_2.png
│   └── image_lettre
│       ├── dezz_25.png
│       ├── erfrf_23.png
│       ├── ezedze_18.png
│       ├── srOovXKa0mmq_3.png
│       └── ze_1.png
└── web
    ├── ayemane.jpg
    ├── index.html
    ├── lucas.jpg
    ├── matyas.jpg
    └── michael.jpg
```

---
## Features

Image Analysis: The image_analyse module is responsible for detecting and extracting letters and words from provided images.

Image Binarization: The image_binarization module prepares images for analysis by resizing and converting them to grayscale.

Letter Learning: The letter_ia folder contains algorithms necessary for training and creating models for letter recognition.

Neurons: The neuron module implements neurons used for machine learning tasks within the project.

Rotation Analysis: The rotation_analysis section includes tools to analyze and adjust images that have been misaligned.

---
## Usage

To compile and run the project, use the provided Makefile. Execute the following command in the terminal to compile both the interface and the solver:

```
make all
``` 

or just:

```
make
```

Once the compilation is complete, you can run the interface using the following command:

```
./interface_main
```

You can also use the solver this way:

```
./solver <grid_file> <word>
```

Then select the option `RESOLVE`, a picture of the grid you want to solve, press `START` and let the magic happen!
