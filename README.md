<h3>Зависимости:</h3>
<ol>
<li>Cmake</li>
<li>Visual Studio</li>
<li>Qt6(msvc2019_64)</li>
<li>Visual C++</li>
</ol>
<h3>Иструкция по сборке:</h3>
<ol>
<li>Клонируйте репозиторий</li>
<li>Создайте папку build ( mkdir build ; cd build)</li>
<li>Сгенерируйте проект для Visual Studio ( cmake ../NBAdmin -DCMAKE_BUILD_TYPE=(1) -DCMAKE_PREFIX_PATH=(2) )<br />3.1) (1) -- Для релиз сборки, установите значение Release<br />3.2) (2) -- Путь до папки lib/cmake ( Пример: C:/Qt/6.4.2/msvc2019_64/lib/cmake )</li>
<li>Соберите проект ( cmake --build ./ --config (1) )<br />4.1) (1) -- Для релиз сборки , установите значение release</li>
<li>Запустите утилиту windeployqt.exe ( windeployqt.exe .\NBAdmin.exe --(1) )<br />5.1) (1) -- Для релиз сборки , установите значение release</li>
<li>Запустите NBAdmin.exe</li>
<li>Если на вашем компьютере нет Visual C++, установите с официвльного сайта microsoft</li>
</ol>
