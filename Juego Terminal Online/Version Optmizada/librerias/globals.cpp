#include "..\encabezados\globals.h"

// Definiciones de variables globales de turno
bool turno = false;
bool first = false;
string valido = "";
string podio = "";

// Definiciones de variables globales de juego
vector<string> letras = {"A", "B", "C", "D", "E", "F", "G", "H", "I"};
vector<int> boats_size = {2, 3, 3, 4, 5}; // TamaÃ±o de los barcos en nÃºmeros

string jugador2_vida = "3";
string jugador2_user = "";

// Definiciones de colores
string red = "\033[1;31m";
string blue = "\033[1;34m";
string reset = "\033[0m";
string green = "\033[1;32m";
string orange = "\033[38;5;208m";
string yellow = "\033[38;5;226m";
string f_yellow = "\033[48;5;226m";
string f_gray = "\033[48;5;7m";
string f_red = "\033[1;41m";

// Imagenes ascii
string barco_imagen = R"(                                                                                         
                                                                                          
                                                                                          
                                                                                          
                                                                                          
                                                                                          
                                                                                          
                                                =                                         
                                               :+ :                                       
                                               :+.:                                       
                                               :=..                                       
                                          :-    =.    -                -                  
                                           =   .+==-  =                *-                 
                                          .+  .+-*+-::=               .=:                 
                                       . .:+==+***++**+                -=                 
                                     .:-   +.  =-*+::=*.   :          .-=                 
                                      :-:. =+-+*=*+-::*=:::+...--.    .-=                 
                                     ---==-=========**+=====--===++.  .-+                 
                                    ==++=+*=##*#***=*+#@#**+%%*%****-  +=                 
                                  :=+=+#*+*=+*+*+++++++++++#***#*##*..:+*+-               
                                 :..===:.-=::*-::+::+-:--:---==+*+=+*##%%#+---:::.        
                             .  .=+:.....:--:-::-+::+-:--:--++*##***********#%***+-       
                            :#+++. . ....::=-:::=*:-+===++****+++++++++++***#**++-        
                           -=*#%+.   ...::=:-====+===+%+==+++++++++++++*******++:         
                           =#%%%+  . ::.:-----=**=++======+++++++++++++******+=.          
                         :.  =*#=  ....::::----=========+++++++++++++++*****+-            
                   :::-=====+*=*+ ....:::----=========+#####+++++++++++****+:             
                 -#=###+++*++=-:..:.:::::----=========+*@@@@*+++++++++****=               
                 %.-%%++***=... ....::::----=========+++#%%#**+++++++****+-               
                 % .+%+**##+  .....::::----========++++++*#+++++++++***+ .+               
                :%---=+==-:.......:::-=---=====+++++++++*#%**+++++++**: :-#=              
                -#   =:... .....::::----===++++++++**+***%%%*+**++++*.  .#@*.             
            ..:.-==-+= ........:::---=-=+++++++++++++++++***+======-=   .::.              
           +:::::............::::-----=============================-+                     
           :-------------===++++****####%%%%%%%@@@@@@@@@@@@%%%%%%%%@@=-==-                
                -*=+%%%%%%@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@%%%%%%%%%%%@@@%%%%+               
                :+   =@%@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@%%%%%%%%%@@@@@*               
                .:    =%%@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@:               
                        :-=++***##%%%@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@-                
                                          .:::----====++**##%%@@@@@@@@%+.                 
                                                                 .:::.                                                                                                            
)";
string moriste = R"(                                                                                          
             .:::.                                                     ::----:            
          .--::::---.                                                :--:::::--           
         :--::...::---                                             .---:....:-=.          
        .=--:.....::--=.                                          .=--::....:-:::..       
     ..:---::::....::--=           ..::-------------::..          ==--:::::::...:::::.    
  .--::::...:::::::::--=:     .:---------------------------:.    .==--:::::.........::-   
 .--::.........::::::---= .:------:::::::::::::::::::::--------: :=---::::....  ...::--:  
 .=-::....   ....:::----------::::......................::::----------:::.........::--=:  
  :=--::.........::-------::::.......             .........::::------:::........::--==-   
    -==---------::------::::.....                       ......::::-----::::::::--====.    
      .-========------::::....                             .....::::----::---===+=:.      
          .:::::----::::....                                 ....:::::---:---::.          
               :---::::....                                   ....::::----:               
              :---:::::....                                    ....::::----:              
              ----:::::....                                     ...::::-----:             
             .----::::.....                                     ....:::------             
             :----:::.......                                   ......:::-----             
             :---::::.......                                   .......::-----             
             .---:::.............:::..              .-====-:..........::---=-             
              ---:::...... :=*%@@@@@@@%*:        .+%@@@@@@@@@%*- .....:::--=:             
              :--:::.... =%@@@@@@@@@@@@@@-      .#@@@@@@@@@@@@@@%-....:::---              
               --::::...#@@@@@%%%%%%%%%@@#      -@@@%%%%%%%%%%@@@@-.:::::-=.              
               .-::::::-@@@%%%%%%%%%%%%%@#      -@%%%%%%%%%%%%%%@@%.:::::--               
                :-:::::=@%%%%%%######%%%@+.......%%%%########%%%%%@.:::---                
                 ---::::%%%%%#########%%%........=%%%###########%%*:::---.                
                  --:::.=%###****####%%#:.........-#%##***+++***##::::--.                 
                  --::::.-****+****##+-...      ....-+***++++++*+:::::--                  
                  --::.....--====--:.....  .-=:   .....:-======:.:::::-=                  
                  :--::.......::::::.....:#@@@@%= ....:::::::::::::::--=                  
                   -=--:::::::::::::....:%%%%#%@@=...::::::::::::::--==.                  
                  .-====--------:::::::::-==-:-==:::::::::-------====-                    
              ..:---------=======--:::::::::::::::::::::---===----:::--:.                 
        .::-==-----------===+=-===--::::::::::::::::::-------==---:::::---====--:.        
     .:--::::-----------====:  -===---::::::::::::::-------::==---:::::::::::::::---:     
    :-::..:::::::::::--===:    :====-----------------===---.  -=---:::::::::.......::-:   
   -::.....::::::::::-===       ========--=======--=====--:    .---:::...::::........::-  
  :-::....::::::...::-==-       -----:.:-:..:-:.:--::--:--:      =-::.......::::.....::-: 
  .=-:::::::::.....:--==:       :=---:..:....:...::::-----       :--::....  ...::::::--=  
   .==-----::......:--==.         ..::::::..:-::::-:-::.          --:::....   ..:-=---:   
     .:-==-:......::--=-                                          :--::........:-:        
          :-:....::--=-                                            :--:::....::--         
           -::::::--=:                                              .---:::::---.         
           .-------:                                                  .:---==-:           
             .....                                                                        
)";

string ganaste = R"(
                              %%##########################%%                              
                              @=                          =@                              
                         #####@=            :-            -@###%#                         
                         @+  .@=           :@@+           -@:  +%                         
                         #%   @+         .-@+-@+..        =@.  %*                         
                         .@=  #%     -%@#**-  :**#@@#     *%  =@.                         
                          =@+ -@:     :*#-      :#%-     .@= =@:                          
                           :%#:##       -@:     @*       *%:*%.                           
                             =%@@+      =%:=##+-*%      -@%#-                             
                               :+@+     #@*=. :+%@.    =@=.                               
                                 :@*.                 +%:                                 
                                   *@=.             =%+                                   
                                    .+@#=.      .-*%+.                                    
                                       :+*@=  -%*=:                                       
                                          @%  *%                                          
                                          @%  *%                                          
                                          @%  *%                                          
                                          @%  *%                                          
                                     #%###@%%%%@%%%%#                                     
                                     @#            *%                                     
                                .++++@%++++++++++++%@++++                                 
                                :@*::::::::::::::::::::=@.                                
                                :@+                    -@.                                
                                :@+                    -@.                                
                       +*##*##*##@%**#**#*##*##*##*##*#%@##*##*##**                       

)";
