<?php   

require 'vendor/autoload.php';
use Ratchet\MessageComponentInterface;
use Ratchet\ConnectionInterface;   

class MyWebSocket implements MessageComponentInterface {
   public $clients;
   private $connectedClients;

   public function __construct() {
      $this->clients = new \SplObjectStorage;
      $this->connectedClients = [];
   }

   public function onOpen( ConnectionInterface $conn) {
      $this->clients->attach($conn);
      $this->connectedClients[$conn->resourceId] = $conn;
      echo "Nova conexão! ({$conn->resourceId})\n";
      $conn->send("Bem-vindo ao servidor WebSocket!");
   } 
   public function onMessage(ConnectionInterface $from, $msg){
      echo "Mensagem recebida de {$from->resourceId}: $msg\n";
      foreach ($this->connectedClients as $client) {
         $client->send($msg);
      }
   }
   public function onClose(ConnectionInterface $conn){
      echo "Conexão fechada. ({$conn->resourceId})\n";
      $conn->close();
   } 
   public function onError(ConnectionInterface $conn, Exception $e){
      echo "Ocorreu um erro: " . $e->getMessage() . "}\n";
      $conn->close();
   } 
}  

$app = new Ratchet\App("172.16.4.13", 81, "0.0.0.0");
$app->route('/', new MyWebSocket, array('*'));

$app->run();
?>