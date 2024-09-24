#include <opencv2/core.hpp>          // Inclui os cabeçalhos principais do OpenCV
#include <opencv2/imgcodecs.hpp>     // Inclui funções para ler e escrever imagens
#include <opencv2/highgui.hpp>       // Inclui funções para exibição de janelas e manipulação de interfaces gráficas
#include <iostream>                  // Inclui a biblioteca padrão de entrada e saída
#include <unistd.h>                  // Inclui funções do sistema POSIX
#include <sys/wait.h>                // Inclui funções para esperar o término dos processos filhos

using namespace cv;                  // Utiliza o namespace do OpenCV
using namespace std;                 // Utiliza o namespace padrão do C++

int main() {
    // Lê a imagem
    Mat img = imread("coins.png", IMREAD_GRAYSCALE); // Lê a imagem "coins.png" em escala de cinza
    
    // Verifica se a imagem foi lida
    if(img.empty()) {
        cout << "Não foi possível ler a imagem: " << "coins.png" << endl; // Mensagem de erro se a imagem não foi lida
        return 1; // Retorna 1 para indicar erro
    }
    
    // Cria as matrizes
    int M[img.rows][img.cols]; // Matriz para armazenar a imagem original
    int Rx[img.rows][img.cols]; // Matriz para armazenar as bordas no eixo x
    int Ry[img.rows][img.cols]; // Matriz para armazenar as bordas no eixo y
    int R[img.rows][img.cols]; // Matriz para armazenar a imagem final

    // Preenche a matriz com os valores da imagem e inicializa as outras matrizes com zero
    for(int i = 0; i < img.rows; i++) {
        for(int j = 0; j < img.cols; j++) {
            M[i][j] = img.at<uchar>(i, j); // Preenche M com os valores da imagem
            Rx[i][j] = 0; // Inicializa Rx com zero
            Ry[i][j] = 0; // Inicializa Ry com zero
            R[i][j] = 0; // Inicializa R com zero
        }
    }

    // Primeiro filho criado usando fork()
    int id = fork(); // Cria um processo filho
    
    if(id == 0) {
        // Executa o algoritmo de detecção de bordas em x no processo filho
        for(int i = 1; i < img.rows - 1; i++) {
            for(int j = 1; j < img.cols - 1; j++) {
                Rx[i][j] = (M[i+1][j-1] + M[i+1][j] + M[i+1][j+1]) - (M[i-1][j-1] + M[i-1][j] + M[i-1][j+1]); // Calcula a borda em x
                if (Rx[i][j] < 0) Rx[i][j] = -Rx[i][j]; // Converte valores negativos para positivos
                if (Rx[i][j] > 255) Rx[i][j] = 255; // Limita os valores ao máximo de 255
            }
        }  

        Mat imgRx(img.rows, img.cols, CV_8UC1); // Cria uma matriz do OpenCV para armazenar a imagem Rx

        for (int i = 0; i < img.rows; ++i) {
            for (int j = 0; j < img.cols; ++j) {
                imgRx.at<uchar>(i, j) = Rx[i][j]; // Preenche imgRx com os valores de Rx
            }
        }

        // Salva a imagem em um arquivo para que possa ser lida no processo pai
        imwrite("Rx.png", imgRx); // Salva imgRx como "Rx.png"

        exit(0); // Encerra o processo filho
    }

    if(id > 0) {   
        // Espera o processo filho terminar para executar o processo pai
        wait(NULL); // Espera o processo filho terminar

        // Cria um novo processo filho
        int id2 = fork(); // Cria outro processo filho     

        if(id2 == 0) {
            // Executa o algoritmo de detecção de bordas em y no novo processo filho
            for(int i = 1; i < img.rows - 1; i++) {
                for(int j = 1; j < img.cols - 1; j++) {
                    Ry[i][j] = (M[i-1][j+1] + M[i][j+1] + M[i+1][j+1]) - (M[i-1][j-1] + M[i][j-1] + M[i+1][j-1]); // Calcula a borda em y
                    if (Ry[i][j] < 0) Ry[i][j] = -Ry[i][j]; // Converte valores negativos para positivos
                    if (Ry[i][j] > 255) Ry[i][j] = 255; // Limita os valores ao máximo de 255
                }
            }

            Mat imgRy(img.rows, img.cols, CV_8UC1); // Cria uma matriz do OpenCV para armazenar a imagem Ry

            for (int i = 0; i < img.rows; ++i) {
                for (int j = 0; j < img.cols; ++j) {
                    imgRy.at<uchar>(i, j) = Ry[i][j]; // Preenche imgRy com os valores de Ry
                }
            }

            // Salva a imagem em um arquivo para que possa ser lida
            imwrite("Ry.png", imgRy); // Salva imgRy como "Ry.png"

            exit(0); // Encerra o processo filho
        }

        if(id2 > 0) {   
            // Espera o processo filho terminar para executar o processo pai
            wait(NULL); // Espera o processo filho terminar

            // Lê ambas as imagens salvas e faz a soma entre elas
            Mat imgRx = imread("Rx.png", IMREAD_GRAYSCALE); // Lê "Rx.png"
            Mat imgRy = imread("Ry.png", IMREAD_GRAYSCALE); // Lê "Ry.png"

            Mat imgR; // Cria uma matriz para armazenar a imagem resultante
            add(imgRx, imgRy, imgR); // Soma as imagens imgRx e imgRy

            // Salva e exibe a imagem resultante
            imwrite("R.png", imgR); // Salva imgR como "R.png"
            imshow("Resultado", imgR); // Exibe imgR em uma janela chamada "Resultado"
            waitKey(0); // Espera uma tecla ser pressionada
        }
    }
    
    return 0; // Retorna 0 para indicar que o programa terminou com sucesso
}
