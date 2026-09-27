# Campo gravitacional em C

Aplicativo desktop escrito inteiramente em C11, com raylib 5.5 e OpenGL por meio da raylib. Sem imagens, modelos, shaders externos ou C++. Uma malha de 80 × 80 células recebe uma depressão central permanente e uma depressão menor que acompanha uma esfera arrastável.

## Executar imediatamente no Windows

Abra `bin/gravity.exe`. O pacote inclui um executável Windows x64 compilado em Release. A raylib está vinculada estaticamente; nenhum recurso externo é necessário. Se o Windows solicitar o runtime do Visual C++, instale o Microsoft Visual C++ Redistributable x64.

Validação realizada: compilação MSVC concluída, teste automatizado da deformação aprovado e execução de oito quadros com OpenGL 3.3 concluída com saída normal. O arraste não foi validado por automação de mouse. Linux e macOS têm instruções de compilação, mas não foram testados neste ambiente. A configuração interna da raylib também detecta um compilador C++; o código deste aplicativo e os módulos compilados são C.

## Dependências

- CMake 3.24 ou superior.
- Compilador C11: Visual Studio 2022/2026 com **Desenvolvimento para desktop com C++** (inclui o compilador C e Windows SDK), GCC ou Clang.
- Driver gráfico com OpenGL 3.3.
- Internet na primeira configuração: o CMake baixa a versão fixa raylib 5.5 e a compila. Não é necessário instalar raylib separadamente.

No Debian/Ubuntu, instale as ferramentas e dependências de desenvolvimento do backend X11:

```sh
sudo apt install build-essential cmake libasound2-dev libx11-dev libxrandr-dev libxi-dev libgl1-mesa-dev libglu1-mesa-dev libxcursor-dev libxinerama-dev
```

No macOS, use as ferramentas de linha de comando do Xcode (`xcode-select --install`) e CMake.

## Compilar no Windows

Abra um terminal na pasta deste README:

```powershell
cmake -S . -B build -A x64
cmake --build build --config Release --parallel
ctest --test-dir build -C Release --output-on-failure
.\build\Release\gravity.exe
```

Se o compilador não for encontrado, use o **Developer PowerShell for Visual Studio** e confirme que o componente de desenvolvimento desktop e o Windows SDK estão instalados.

## Compilar no Linux/macOS

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel
ctest --test-dir build --output-on-failure
./build/gravity
```

Para usar uma instalação de raylib 5.5 ou compatível, acrescente `-DGRAVITY_SYSTEM_RAYLIB=ON` à configuração. Para usar fontes já baixadas sem internet, forneça `-DFETCHCONTENT_SOURCE_DIR_RAYLIB=/caminho/para/raylib`.

## Controles

| Entrada | Ação |
| --- | --- |
| Botão esquerdo sobre a esfera azul + arrastar | Move a esfera pela malha |
| Botão direito + arrastar | Orbita a câmera |
| Roda do mouse | Aproxima/afasta |
| R | Restaura esfera e câmera |
| W | Alterna superfície preenchida e apenas linhas |
| Esc | Fecha |

A esfera fica dourada durante o arraste. Sua posição horizontal é limitada ao interior da malha; a altura acompanha a superfície deformada. Ao soltar, ela permanece onde foi colocada. Não há queda física, atração automática ou colisões. O zoom e a órbita ficam suspensos durante o arraste para evitar deslocamentos inesperados.

## Como funciona

`src/field.c` calcula a altura em cada ponto:

```text
y = -4.2 exp(-(x² + z²)/9)
    -1.15 exp(-((x-bx)² + (z-bz)²)/2)
```

O primeiro termo forma o poço central. O segundo acompanha a posição `(bx, bz)` da esfera. São funções suaves, finitas e puramente visuais. As depressões se somam quando a esfera passa pelo centro. O marcador preto com aro laranja identifica o buraco negro.

`src/main.c` desenha a superfície e suas linhas a cada quadro. O clique usa um raio da câmera contra a esfera; o arraste intersecta esse raio com um plano horizontal fixado no começo do gesto. Um deslocamento inicial preserva o ponto de pega e evita um salto inicial. Como a altura da esfera muda, ela pode se afastar verticalmente do cursor durante o gesto: o cursor controla a posição horizontal no plano de referência.

`tests/field_test.c` verifica profundidade central, deslocamento da depressão móvel, simetria, limite de profundidade e ausência de singularidades. `gravity --smoke-test` abre a janela, renderiza oito quadros e sai para uma verificação breve do contexto gráfico.

## Verificação manual

1. Arraste a esfera para os quatro lados: o poço menor deve acompanhá-la e a região anterior deve se recuperar.
2. Leve-a ao centro: os dois poços devem se somar sem valores infinitos.
3. Orbite, aplique zoom e repita o arraste; teste também os limites da malha.
4. Alterne W, redimensione a janela e pressione R.

## Referências

- [raylib 5.5 — API oficial](https://github.com/raysan5/raylib/blob/5.5/src/raylib.h)
- [raylib — compilação com CMake](https://github.com/raysan5/raylib/wiki/Working-with-CMake)
- [raylib — dependências GNU/Linux](https://github.com/raysan5/raylib/wiki/Working-on-GNU-Linux)

A raylib possui licença zlib/libpng, incluída nas fontes baixadas pelo CMake.
