#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <HTTPClient.h>
#include <WebServer.h> // Biblioteca nativa para o site

// ================= CONFIGURAÇÕES DO USUÁRIO =================
#define WIFI_SSID     "Nome da rede"
#define WIFI_PASSWORD "Senha da rede"
#define BOT_TOKEN     "Token do bot"
#define CHAT_ID       "Seu ID no Telegram" // Use o IDBot para pegar
#define GEMINI_API_KEY "Sua Key da API do Gemini"

// Configurações de Tempo
const unsigned long BOT_MTBS = 1000; // Checar telegram a cada 1s

// ================= OBJETOS E VARIÁVEIS =================
WiFiClientSecure clientBot;
UniversalTelegramBot bot(BOT_TOKEN, clientBot);
Adafruit_BMP280 bmp;
WebServer server(80); // O Site roda na porta 80

unsigned long lastTimeBotRan;
String userProfile = "Estudante padrao"; 

// Variáveis para guardar a última leitura (Para mostrar no site)
float lastTemp = 0.0;
float lastPress = 0.0;
String lastInsight = "Aguardando sincronização...";

// ================= HTML DO SITE =================
String getHTML() {
  String ptr = "<!DOCTYPE html><html><head>";
  ptr += "<meta charset='UTF-8'>"; // OBRIGATÓRIO para acentos e emojis
  ptr += "<meta name='viewport' content='width=device-width, initial-scale=1'>";
  ptr += "<meta http-equiv='refresh' content='30'>"; // Atualiza a cada 30s
  ptr += "<title>MindCast</title>";
  ptr += "<style>";
  
  // 1. Fundo: Um degradê escuro
  ptr += "body { background: linear-gradient(135deg, #102027 0%, #263238 50%, #006064 100%); color: #ffffff; font-family: 'Segoe UI', Roboto, Helvetica, sans-serif; margin: 0; padding: 20px; min-height: 100vh; display: flex; flex-direction: column; align-items: center; justify-content: center; box-sizing: border-box; }";
  
  // 2. Título MindCast
  ptr += "h1 { font-weight: 700; letter-spacing: 2px; margin-bottom: 5px; background: -webkit-linear-gradient(45deg, #4DD0E1, #FFFFFF); -webkit-background-clip: text; -webkit-text-fill-color: transparent; text-transform: uppercase; font-size: 2.5rem; text-align: center; }";
  
  // 3. Slogan (Texto do Telegram)
  ptr += "p.slogan { color: #B2EBF2; font-size: 1.1rem; margin-top: 0px; margin-bottom: 40px; text-align: center; font-weight: 300; max-width: 600px; line-height: 1.4; }";
  
  // Container dos Cards
  ptr += ".container { display: flex; flex-wrap: wrap; justify-content: center; gap: 20px; max-width: 900px; width: 100%; margin-bottom: 40px; }";
  
  // Design dos Cards (Vidro Fosco / Glassmorphism)
  ptr += ".card { background: rgba(255, 255, 255, 0.05); backdrop-filter: blur(10px); -webkit-backdrop-filter: blur(10px); border: 1px solid rgba(77, 208, 225, 0.2); border-radius: 20px; padding: 25px; text-align: center; flex: 1; min-width: 200px; box-shadow: 0 8px 32px 0 rgba(0, 0, 0, 0.3); transition: transform 0.3s ease; }";
  ptr += ".card:hover { transform: translateY(-5px); border-color: #4DD0E1; }";

  // Tipografia dos Dados
  ptr += "h2 { font-size: 0.9rem; color: #80DEEA; text-transform: uppercase; letter-spacing: 1px; margin: 0; opacity: 0.9; margin-bottom: 10px; }";
  ptr += ".value { font-size: 3.5rem; font-weight: 700; margin: 0; color: #FFFFFF; text-shadow: 0 0 10px rgba(77, 208, 225, 0.5); }";
  ptr += ".unit { font-size: 1.2rem; color: #4DD0E1; vertical-align: super; font-weight: 400; }";
  
  // Card do Insight (Destaque)
  ptr += ".insight-card { width: 100%; text-align: left; border-left: 5px solid #4DD0E1; min-width: 300px; flex-basis: 100%; background: rgba(0, 96, 100, 0.3); }";
  ptr += ".insight-text { font-style: italic; font-size: 1.2rem; line-height: 1.5; color: #E0F7FA; font-weight: 300; }";
  ptr += ".insight-label { color: #4DD0E1; font-weight: bold; font-size: 0.8rem; display: block; margin-bottom: 10px; text-transform: uppercase; letter-spacing: 1px;}";
  
  // 4. Rodapé com Instruções (Igual ao Telegram)
  ptr += ".footer-info { background: rgba(0,0,0,0.2); border-radius: 15px; padding: 20px; max-width: 600px; width: 100%; text-align: left; border: 1px dashed #546E7A; }";
  ptr += ".footer-info h3 { color: #4DD0E1; font-size: 1rem; margin-top: 0; }";
  ptr += ".footer-info p { color: #B0BEC5; font-size: 0.9rem; margin: 5px 0; }";
  ptr += ".code { font-family: monospace; background: #263238; padding: 2px 6px; border-radius: 4px; color: #80CBC4; }";
  
  ptr += "</style></head><body>";
  
  // --- CONTEÚDO VISUAL ---
  ptr += "<h1>MindCast AI</h1>";
  ptr += "<p class='slogan'>🧠☁️ Sincronize sua biologia com o clima ao seu redor.<br>Vamos usar o ambiente a seu favor?</p>";
  
  ptr += "<div class='container'>";
  // Card Temperatura
  ptr += "<div class='card'><h2>🌡️ Temperatura</h2><div class='value'>" + String(lastTemp, 0) + "<span class='unit'>&deg;C</span></div></div>";
  // Card Pressão
  ptr += "<div class='card'><h2>☁️ Pressão</h2><div class='value'>" + String(lastPress, 0) + "<span class='unit'>hPa</span></div></div>";
  // Insight IA
  ptr += "<div class='card insight-card'><span class='insight-label'>☁️ Insight Ambiental</span><div class='insight-text'>&quot;" + lastInsight + "&quot;</div></div>";
  ptr += "</div>";
  
  // Instruções no Rodapé (Espelhando o Bot)
  ptr += "<div class='footer-info'>";
  ptr += "<h3>📱 Primeiros Passos no Telegram:</h3>";
  ptr += "<p>1️⃣ Diga seu contexto: <span class='code'>/perfil [Seu status]</span></p>";
  ptr += "<p>    Exemplo: <span class='code'>/perfil Estudante focado nos estudos</span></p>";
  ptr += "<p>2️⃣ Receba seu insight: <span class='code'>/agora</span></p>";
  ptr += "<p style='margin-top:15px; font-size:0.8rem; text-align:center; opacity:0.6;'>MindCast IoT System &bull; Powered by Gemini 2.0</p>";
  ptr += "</div>";
  
  ptr += "</body></html>";
  return ptr;
}

// ================= FUNÇÃO GEMINI =================
String callGemini(float temp, float press, String perfil) {
  if (perfil == "" || perfil == "null") perfil = "Estudante universitario";
  
  WiFiClientSecure clientGoogle;
  clientGoogle.setInsecure();
  HTTPClient https;
  
  // URL GEMINI 2.0 FLASH
  String url = "https://generativelanguage.googleapis.com/v1beta/models/gemini-2.0-flash:generateContent?key=" + String(GEMINI_API_KEY);
  
  // PROMPT COACH AMBIENTAL
  String prompt = "Aja como o MindCast, um coach de performance e bem-estar. ";
  prompt += "O perfil do usuário é: '" + perfil + "'. ";
  prompt += "Ambiente: Temperatura " + String(temp, 0) + "°C, Pressão " + String(press, 0) + "hPa. ";
  
  prompt += "DIRETRIZES: ";
  prompt += "1. NUNCA repita o nome do perfil (ex: não diga 'Olá empresário'). ";
  prompt += "2. Cite o dado ambiental arredondado como contexto. ";
  prompt += "3. Responda em 3 frases: Impacto biológico -> Ação de produtividade -> Micro-hábito de saúde. ";
  prompt += "4. Termine com um emoji.";

  DynamicJsonDocument doc(1024);
  JsonObject part = doc.createNestedArray("contents").createNestedObject().createNestedArray("parts").createNestedObject();
  part["text"] = prompt;
  String jsonPayload;
  serializeJson(doc, jsonPayload);

  if (https.begin(clientGoogle, url)) {
      https.addHeader("Content-Type", "application/json");
      int httpCode = https.POST(jsonPayload);
      if (httpCode > 0) {
        String response = https.getString();
        DynamicJsonDocument docRes(2048);
        deserializeJson(docRes, response);
        if (!docRes["candidates"][0]["content"]["parts"][0]["text"].isNull()) {
            https.end();
            return docRes["candidates"][0]["content"]["parts"][0]["text"].as<String>();
        }
      }
      https.end();
  }
  return "Erro na conexão com IA. Tente novamente.";
}

// ================= HANDLERS TELEGRAM COM BOTÕES E SEU TEXTO =================
void handleNewMessages(int numNewMessages) {
  for (int i = 0; i < numNewMessages; i++) {
    String chat_id = String(bot.messages[i].chat_id);
    String text = bot.messages[i].text;

    // --- DEFINIÇÃO DOS BOTÕES (JSON) ---
    // Cria um botão grande "/agora" em cima e dois menores embaixo
    String teclado = "[[\"/agora\"], [\"/perfil\", \"/ajuda\"]]";

    if (text == "/start" || text == "/ajuda") {
      String welcome = "Olá! Eu sou o MindCast 🧠☁️\n\n";
      welcome += "Estou aqui para sincronizar sua biologia com o clima ao seu redor.\n";
      welcome += "*Vamos usar o ambiente a seu favor?*\n\n";
      welcome += "Primeiros passos:\n1️⃣ Me diga seu contexto: /perfil [Seu status] \n    Exemplo: `/perfil Estudante focado` \n";
      welcome += "2️⃣ Receba seu insight agora mesmo: /agora";
      welcome += "\n🌐 Insights disponíveis na Web em: http://" + WiFi.localIP().toString();
      
      // Envia mensagem COM o teclado de botões ativado
      bot.sendMessageWithReplyKeyboard(chat_id, welcome, "Markdown", teclado, true);
    }
    
    // Lógica inteligente para o comando /perfil
    else if (text.startsWith("/perfil")) {
      // Caso 1: Usuário só clicou no botão "/perfil" (sem digitar nada)
      if (text == "/perfil") {
        bot.sendMessage(chat_id, "⚠️ Para configurar, digite o comando completo.\nExemplo: `/perfil Estou cansado e estudando`", "Markdown");
      } 
      // Caso 2: Usuário digitou o perfil corretamente
      else {
        userProfile = text.substring(8);
        // Confirma e mostra os botões de novo
        bot.sendMessageWithReplyKeyboard(chat_id, "✅ Algoritmo calibrado! Contexto definido como: *" + userProfile + "*\nAgora clique em /agora!", "Markdown", teclado, true);
      }
    }
    
    else if (text == "/agora") {
      bot.sendMessage(chat_id, "🔄 Sincronizando dados ambientais e calculando bio-impacto...", "");
      
      // Lê sensores
      lastTemp = bmp.readTemperature();
      lastPress = bmp.readPressure() / 100.0F;
      
      // Chama IA
      lastInsight = callGemini(lastTemp, lastPress, userProfile);
      
      // Envia Resposta mantendo o teclado na tela
      bot.sendMessageWithReplyKeyboard(chat_id, lastInsight, "", teclado, true);
    }
    
    else {
      // Se digitar algo estranho, mostra os botões para ajudar
      bot.sendMessageWithReplyKeyboard(chat_id, "Comando não reconhecido. Use os botões abaixo. 👇", "", teclado, true);
    }
  }
}

// ================= SETUP =================
void setup() {
  Serial.begin(115200);
  
  // Inicia Sensor I2C (Endereço 0x76 ou 0x77)
  if (!bmp.begin(0x76)) {
    if (!bmp.begin(0x77)) {
       Serial.println("ERRO BMP280");
    }
  }

  // Conecta Wi-Fi
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  clientBot.setInsecure();
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nIP WebServer: " + WiFi.localIP().toString());
  
  // Inicia Site
  server.on("/", []() {
    server.send(200, "text/html", getHTML());
  });
  server.begin();

  // Avisa no Telegram que ligou
  bot.sendMessage(CHAT_ID, "✨ **MindCast Online.**\nSensores atmosféricos ativos. Painel: http://" + WiFi.localIP().toString(), "Markdown");
}

// ================= LOOP PRINCIPAL =================
void loop() {
  // 1. Mantém o Site Vivo
  server.handleClient();

  // 2. Mantém o Telegram Vivo (Sem travar o site)
  if (millis() - lastTimeBotRan > BOT_MTBS) {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    while (numNewMessages) {
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
}