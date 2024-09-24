#include <iostream> // Biblioteca para entrada e saída padrão
#include <opencv2/core.hpp> // Biblioteca do OpenCV para manipulação de estruturas básicas
#include <opencv2/imgcodecs.hpp> // Biblioteca do OpenCV para codificação e decodificação de imagens
#include <opencv2/highgui.hpp> // Biblioteca do OpenCV para interfaces gráficas
#include <opencv2/imgproc.hpp> // Biblioteca do OpenCV para processamento de imagens
#include <thread> // Biblioteca para manipulação de threads
#include <vector> // Biblioteca para uso de vetores dinâmicos

using namespace std; // Usa o espaço de nomes padrão para simplificar o código
using namespace cv; // Usa o espaço de nomes do OpenCV para simplificar o código

// Matriz global para armazenar os valores da imagem original
vector<vector<int>> M;

// Função que faz a detecção das bordas no eixo x e retorna a imagem processada
void bordaX(const vector<vector<int>>& IMG, Mat& imgRx, int n, int m) {
    // Cria uma matriz local para armazenar os valores da borda x
    vector<vector<int>> Rx(n, vector<int>(m, 0));

    // Calcula a borda em x usando o operador de Prewitt
    for(int i = 1; i < n - 1; ++i) {
        for(int j = 1; j < m - 1; ++j) {
            Rx[i][j] = (IMG[i+1][j-1] + IMG[i+1][j] + IMG[i+1][j+1]) - 
                       (IMG[i-1][j-1] + IMG[i-1][j] + IMG[i-1][j+1]);
        }
    }

    // Cria uma imagem OpenCV a partir da matriz Rx
    imgRx = Mat(n, m, CV_8SC1);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            imgRx.at<char>(i, j) = Rx[i][j];
        }
    }

    // Normaliza a imagem para o intervalo 0-255
    normalize(imgRx, imgRx, 0, 255, NORM_MINMAX, CV_8UC1);
}

// Função que faz a detecção das bordas no eixo y e retorna a imagem processada
void bordaY(const vector<vector<int>>& IMG, Mat& imgRy, int n, int m) {
    // Cria uma matriz local para armazenar os valores da borda y
    vector<vector<int>> Ry(n, vector<int>(m, 0));

    // Calcula a borda em y usando o operador de Prewitt
    for(int i = 1; i < n - 1; ++i) {
        for(int j = 1; j < m - 1; ++j) {
            Ry[i][j] = (IMG[i-1][j+1] + IMG[i][j+1] + IMG[i+1][j+1]) - 
                       (IMG[i-1][j-1] + IMG[i][j-1] + IMG[i+1][j-1]);
        }
    }

    // Cria uma imagem OpenCV a partir da matriz Ry
    imgRy = Mat(n, m, CV_8SC1);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < m; ++j) {
            imgRy.at<char>(i, j) = Ry[i][j];
        }
    }

    // Normaliza a imagem para o intervalo 0-255
    normalize(imgRy, imgRy, 0, 255, NORM_MINMAX, CV_8UC1);
}

int main() {
    // Lê a imagem em escala de cinza
    Mat img = imread("coins.png", IMREAD_GRAYSCALE);

    // Verifica se a imagem foi lida corretamente
    if(img.empty()) {
        cout << "Não foi possível ler a imagem: " << "coins.png" << endl;
        return 1;
    }

    // Obtém as dimensões da imagem
    int rows = img.rows;
    int cols = img.cols;

    // Redimensiona a matriz global M para corresponder às dimensões da imagem
    M.resize(rows, vector<int>(cols));
    
    // Preenche a matriz M com os valores dos pixels da imagem
    for(int i = 0; i < rows; ++i) {
        for(int j = 0; j < cols; ++j) {
            M[i][j] = img.at<uchar>(i, j);
        }
    }

    // Variáveis para armazenar as imagens de borda x e y
    Mat imgRx, imgRy;

    // Cria as threads para executar as funções de detecção de borda
    thread t1(bordaX, cref(M), ref(imgRx), rows, cols);
    thread t2(bordaY, cref(M), ref(imgRy), rows, cols);

    // Espera as threads terminarem
    t1.join();
    t2.join();

    // Combina as bordas de x e y
    Mat imgR;
    add(imgRx, imgRy, imgR);

    // Normaliza a imagem resultante para o intervalo 0-255
    normalize(imgR, imgR, 0, 255, NORM_MINMAX, CV_8UC1);

    // Salva a imagem resultante
    imwrite("R.png", imgR);

    return 0;
}