#include <iostream>
#include <stdio.h>
#include <IL/il.h>
#include <vector>
#define IMAGE_NAME "/home/maria_soroka/CLionProjects/Image/Task01b.jpg"
#define FILE_TO_SAVE_GREY "/home/maria_soroka/CLionProjects/Image/smoothed.jpg"
#define FILE_TO_SAVE_BLACK_AND_WHITE "/home/maria_soroka/CLionProjects/Image/black_and_white.jpg"
#define FILE_TO_SAVE_COLORED "/home/maria_soroka/CLionProjects/Image/colored.jpg"
#define FILE_TO_SAVE_RESULTS "/home/maria_soroka/CLionProjects/Image/result.csv"
#define CONSTANT_FOR_MEDIAN_FILTER 1 //this value stands for half-size of the rectangle, which pixels used to count average for smoothing
#define BOUND_FOR_FIGURE_TYPE 0.11 //this value should be less than 1.2, оr bounds for different figure types would intersect
#define CONSTANT_FOR_THE_BORDER 1.05
#define PI 3.14
#define CONSTANT_FROM_EXPIREMENTS_WITH_IDEAL_FIGURES 0.9
enum colors{black, white_already_visited, white, red, green, blue};
enum directions{north, west, south, east};
struct GreyMatrix{
    ILfloat * matrix;
    unsigned int width;
    unsigned int height;
};
struct ColoredMatrix{
    int* matrix;
    unsigned int width;
    unsigned int height;
};
struct NeedsToBeColored{
    int y;
    int left_x;
    int right_x;
};
struct CountFigures{
    int number_of_triangles = 0;
    int number_of_circles = 0;
    int number_of_rectangles = 0;
};
typedef struct GreyMatrix GreyMatrix;
typedef struct ColoredMatrix ColoredMatrix;
typedef struct NeedsToBeColored NeedsToBeColored;
typedef struct CountFigures CountFigures;
GreyMatrix* image_to_grey_matrix(const char * file_name){
    ILuint ImageName;
    ilGenImages(1, &ImageName);
    ilBindImage(ImageName);
    ilLoad(IL_JPG, file_name);
    GreyMatrix* result = (GreyMatrix*)malloc(sizeof(GreyMatrix));
    result->width = ilGetInteger(IL_IMAGE_WIDTH);
    result->height = ilGetInteger(IL_IMAGE_HEIGHT);
    result->matrix = (ILfloat *)malloc(sizeof(ILfloat) * result->height * result->width);
    ILfloat * pixel = (ILfloat *)malloc(ilGetInteger(IL_IMAGE_BPP)*ilGetInteger(IL_IMAGE_BPC));
    for(int i = result->height - 1; i >= 0; i--){
        for(int j = 0; j < result->width; j++){
            ilCopyPixels(j, i, 0, 1, 1, 1, IL_RGB,
                         IL_FLOAT, pixel);
            result->matrix[(result->height - 1 - i)*result->width + j] = (*pixel);
        }
    }
    free(pixel);
    ilDeleteImage(1);
    return result;
}
void grey_matrix_to_image(GreyMatrix* input, const char * filename){
    ILuint ImageName;
    ilGenImages(1, &ImageName);
    ilBindImage(ImageName);
    ilTexImage(input->width, input->height, 0, 3, IL_RGB, IL_FLOAT, NULL);
    int size = ilGetInteger(IL_IMAGE_BPP)*ilGetInteger(IL_IMAGE_BPC);
    ILfloat * data = (ILfloat *)malloc(size * sizeof(ILfloat) * input->height * input->width);
    ILfloat color;
    for(int i = input->height - 1; i >= 0; i--){
        for(int j = 0; j < input->width; j++){
            color= input->matrix[(i) * input->width + j];
            data[size * ((input->height - 1 - i) * input->width + j)] = color;
            data[size * ((input->height - 1 - i) * input->width + j) + 1] = color;
            data[size * ((input->height - 1 - i) * input->width + j) + 2] = color;
            ilSetPixels(j, i, 0, 1, 1, 1, IL_RGB,
                         IL_FLOAT, data + size * (((input->height - 1 - i) * input->width + j)));
        }
    }
    ilSave(IL_JPG, filename);
    free(data);
    ilDeleteImage(1);
}
void colored_matrix_to_image(ColoredMatrix* input, const char * file_name){
    ILuint ImageName;
    ilGenImages(1, &ImageName);
    ilBindImage(ImageName);
    ilTexImage(input->width, input->height, 0, 3, IL_RGB, IL_UNSIGNED_BYTE, NULL);
    int size = ilGetInteger(IL_IMAGE_BPP)*ilGetInteger(IL_IMAGE_BPC);
    ILubyte * data = (ILubyte *)malloc(size * sizeof(ILubyte) * input->height * input->width);
    int color;
    for(int i = input->height - 1; i >= 0; i--){
        for(int j = 0; j < input->width; j++){
            color= input->matrix[(i) * input->width + j];
            if(color == black){
                data[size * ((input->height - 1 - i) * input->width + j)] = 0;
                data[size * ((input->height - 1 - i) * input->width + j) + 1] = 0;
                data[size * ((input->height - 1 - i) * input->width + j) + 2] = 0;
            }
            else if(color == white || color == white_already_visited){
                data[size * ((input->height - 1 - i) * input->width + j)] = 255;
                data[size * ((input->height - 1 - i) * input->width + j) + 1] = 255;
                data[size * ((input->height - 1 - i) * input->width + j) + 2] = 255;
            }
            else if(color == red) {
                data[size * ((input->height - 1 - i) * input->width + j)] = 255;
                data[size * ((input->height - 1 - i) * input->width + j) + 1] = 0;
                data[size * ((input->height - 1 - i) * input->width + j) + 2] = 0;
            }
            else if(color == green){
                data[size * ((input->height - 1 - i) * input->width + j)] = 0;
                data[size * ((input->height - 1 - i) * input->width + j) + 1] = 255;
                data[size * ((input->height - 1 - i) * input->width + j) + 2] = 0;
            }
            else{
                data[size * ((input->height - 1 - i) * input->width + j)] = 0;
                data[size * ((input->height - 1 - i) * input->width + j) + 1] = 0;
                data[size * ((input->height - 1 - i) * input->width + j) + 2] = 255;
            }
            ilSetPixels(j, i, 0, 1, 1, 1, IL_RGB,
                        IL_UNSIGNED_BYTE, data + size * (((input->height - 1 - i) * input->width + j)));
        }
    }
    ilSave(IL_JPG, file_name);
    free(data);
    ilDeleteImage(1);
}

GreyMatrix* average_filter(GreyMatrix* input, int k){//sets pixel color value equal to the average of colors around
    GreyMatrix *result = (GreyMatrix*)malloc(sizeof(GreyMatrix));
    result->width = input->width;
    result->height = input->height;
    result->matrix = (ILfloat *)malloc(sizeof(ILfloat) * result->height * result->width);
    for(int i = 0; i < input->height; i++){
        for(int j = 0; j < input->width; j++){
            float sum = 0;
            int number = 0;
            for(int a = i - k; a <= i + k; a++){
                for(int b = j - k; b <= j + k; b++){
                    if(a >= 0 && b >= 0 && a <= input->height - 1 && b <= input->width - 1){
                        sum += input->matrix[a * (input->width) + b];
                        number++;
                    }
                }
            }
            result->matrix[i * result->width + j] = sum/number;
        }
    }
    return result;
}
ColoredMatrix * make_black_and_white(GreyMatrix* input){
    ColoredMatrix *result = (ColoredMatrix*)malloc(sizeof(ColoredMatrix));
    result->width = input->width;
    result->height = input->height;
    result->matrix = (int *)malloc(sizeof(int) * result->height * result->width);
    float average = 0;//find average value to define border
    for(int i = 0; i < input->height; i++) {
        float temp_average = 0;
        for(int j = 0; j < input->width; j++) {
            temp_average += input->matrix[i * (input->width) + j];
        }
        average += temp_average / input->width;
    }
    average /= input->height;
    average *= CONSTANT_FOR_THE_BORDER;//there is a lot of dark pixels and border is too small to avoid "single" pixels in the background(I believe there is better way to set border value)
    //std::cout << "average "<< average << "\n";
    for(int i = 0; i < input->height; i++) {
        for(int j = 0; j < input->width; j++) {
            if(input->matrix[i * input->width + j] > average) {
                result->matrix[i * result->width + j] = white;
            }
            else {
                result->matrix[i * result->width + j] = black;
            }
        }
    }
    return result;
}
void color_the_picture(ColoredMatrix* input, CountFigures* number_of_figures){
    for(int i = 0; i < input->height; i++){
        for(int j = 0; j < input->width; j++){
            if(input->matrix[i * input->width + j] == white){//bug algorithm
                int color;
                int current_direction = east;
                int current_y = i;
                int current_x = j;
                double perimeter = 0;
                double square = 0;
                std::vector<NeedsToBeColored*> for_future_coloring;
                do{
                   if(input->matrix[current_y * input->width + current_x] == white) {
                       int number_of_black_pixels = 0;//counting number of black neighbours
                       if(input->matrix[current_y * input->width + current_x + 1] == black){
                           number_of_black_pixels++;
                       }
                       if(input->matrix[current_y * input->width + current_x - 1] == black){
                           number_of_black_pixels++;
                       }
                       if(input->matrix[(current_y - 1) * input->width + current_x] == black){
                           number_of_black_pixels++;
                       }
                       if(input->matrix[(current_y + 1) * input->width + current_x] == black){
                           number_of_black_pixels++;
                       }
                       if(number_of_black_pixels == 3 || number_of_black_pixels == 1){
                           perimeter += 1;
                       }
                       else if(number_of_black_pixels == 2){
                           perimeter += 1.4;//to smooth the edges
                       }
                       if(input->matrix[current_y * input->width + current_x - 1] != white && input->matrix[current_y * input->width + current_x - 1] != white_already_visited) {
                           int temp = current_x;
                           while(temp < input->width && (input->matrix[current_y * input->width + temp] == white || input->matrix[current_y * input->width + temp] == white_already_visited)) {
                               square++;
                               temp++;
                           }
                           NeedsToBeColored *row = (NeedsToBeColored *) malloc(sizeof(NeedsToBeColored));
                           row->left_x = current_x;
                           row->right_x = temp - 1;
                           row->y = current_y;
                           for_future_coloring.push_back(row);
                       }
                       current_direction++;
                       current_direction = current_direction % 4;
                       input->matrix[current_y * input->width + current_x] = white_already_visited;
                   }
                   else if(input->matrix[current_y * input->width + current_x] == white_already_visited) {
                       current_direction++;
                       current_direction = current_direction % 4;
                   }
                   else {
                       current_direction += 3; //3(mod 4) = -1(mod 4)
                       current_direction = current_direction % 4;
                   }
                   if(current_direction == north){
                       current_y--;
                   }
                   else if(current_direction == west){
                       current_x--;
                   }
                   else if(current_direction == south){
                       current_y++;
                   }
                   else{
                       current_x++;
                   }
                }while(current_x != j || current_y != i);
                std::cout << perimeter << " " << square << " " << square / perimeter << "\n";
                if((perimeter * perimeter) * CONSTANT_FROM_EXPIREMENTS_WITH_IDEAL_FIGURES >= (1 - BOUND_FOR_FIGURE_TYPE) * 16 * square && perimeter * perimeter * 0.9 <= (1 + BOUND_FOR_FIGURE_TYPE) * 16 * square){
                    number_of_figures->number_of_rectangles++;//it is a rectangle
                    color = blue;

                }
                else if((perimeter * perimeter) * CONSTANT_FROM_EXPIREMENTS_WITH_IDEAL_FIGURES >= (1 - BOUND_FOR_FIGURE_TYPE) * 4 * PI * square && perimeter * perimeter * 0.9 <= (1 + BOUND_FOR_FIGURE_TYPE) * 4 * PI * square){
                    number_of_figures->number_of_circles++;//it is a circle
                    color = red;
                }
                else if((perimeter * perimeter) * CONSTANT_FROM_EXPIREMENTS_WITH_IDEAL_FIGURES > (1 + BOUND_FOR_FIGURE_TYPE) * 4 * PI * square){
                    number_of_figures->number_of_triangles++;//it is a triangle
                    color = green;
                }
                else if(square == 1){
                    color = black;
                }
                else{
                    color = white_already_visited;
                }
                for(int k = 0; k < for_future_coloring.size(); k++){//coloring
                    for(int temp = for_future_coloring[k]->left_x; temp <= for_future_coloring[k]->right_x; temp++){
                        input->matrix[for_future_coloring[k]->y * input->width + temp] = color;
                    }
                }
                for(int k = 0; k < for_future_coloring.size(); k++){
                    free(for_future_coloring[k]);
                }
            }
        }
    }
}
ColoredMatrix* expand(ColoredMatrix* input){//adding black frame, bug algorithm would not leave the image
    ColoredMatrix* result = (ColoredMatrix *)malloc(sizeof(ColoredMatrix*));
    result->width = input->width + 2;
    result->height = input->height + 2;
    result->matrix = (int *)malloc(sizeof(int) * result->height * result->width);
    for(int i = 0; i < result->width; i++){
        result->matrix[i] = black;
        result->matrix[(result->height - 1) * result->width + i] = black;
    }
    for(int i = 0; i < result->height; i++){
        result->matrix[i * result->width] = black;
        result->matrix[i * result->width + result->width - 1] = black;
    }
    for(int i = 1; i < result->height - 1; i++){
        for(int j = 1; j < result->width - 1; j++){
            result->matrix[i*result->width + j] = input->matrix[(i - 1) * input->width + (j - 1)];
        }
    }
    return result;
}
ColoredMatrix* crop(ColoredMatrix* input){
    ColoredMatrix* result = (ColoredMatrix *)malloc(sizeof(ColoredMatrix*));
    result->width = input->width - 2;
    result->height = input->height - 2;
    result->matrix = (int *)malloc(sizeof(int) * result->height * result->width);
    for(int i = 0; i < result->height; i++){
        for(int j = 0; j < result->width; j++){
            result->matrix[i*result->width + j] = input->matrix[(i + 1) * input->width + (j + 1)];
        }
    }
    return result;
}
int main() {
    ilInit();
    ilEnable(IL_FILE_OVERWRITE);
    GreyMatrix * image = image_to_grey_matrix(IMAGE_NAME);
    GreyMatrix* smoothed = average_filter(image, CONSTANT_FOR_MEDIAN_FILTER);
    grey_matrix_to_image(smoothed, FILE_TO_SAVE_GREY);
    ColoredMatrix* black_and_white = make_black_and_white(smoothed);
    colored_matrix_to_image(black_and_white, FILE_TO_SAVE_BLACK_AND_WHITE);
    ColoredMatrix* expanded_black_and_white = expand(black_and_white);
    CountFigures* number_of_figures = (CountFigures*)malloc(sizeof(CountFigures));
    color_the_picture(expanded_black_and_white, number_of_figures);
    ColoredMatrix* cropped_colored = crop(expanded_black_and_white);
    colored_matrix_to_image(cropped_colored, FILE_TO_SAVE_COLORED);
    FILE* result = fopen(FILE_TO_SAVE_RESULTS, "w");
    fprintf(result, "circles: %d\ntriangles: %d\nrectangles: %d\n", number_of_figures->number_of_circles, number_of_figures->number_of_triangles, number_of_figures->number_of_rectangles);
    fclose(result);
    free(image->matrix);
    free(smoothed->matrix);
    free(black_and_white->matrix);
    free(expanded_black_and_white->matrix);
    free(cropped_colored->matrix);
    free(image);
    free(smoothed);
    free(black_and_white);
    free(expanded_black_and_white);
    free(cropped_colored);
    free(number_of_figures);
    return 0;
}