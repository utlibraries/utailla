<?php 

// require_once '../vendor/autoload.php';
// Do this until we expose these in a module or library.
@include_once 'sites/all/libraries/tuque/Datastream.php';
@include_once 'sites/all/libraries/tuque/FedoraApi.php';
@include_once 'sites/all/libraries/tuque/FedoraApiSerializer.php';
@include_once 'sites/all/libraries/tuque/FoxmlDocument.php';
@include_once 'sites/all/libraries/tuque/Object.php';
@include_once 'sites/all/libraries/tuque/HttpConnection.php';
@include_once 'sites/all/libraries/tuque/Cache.php';
@include_once 'sites/all/libraries/tuque/Repository.php';
@include_once 'sites/all/libraries/tuque/RepositoryConnection.php';
@include_once 'sites/all/libraries/tuque/RepositoryQuery.php';
@include_once 'sites/all/libraries/tuque/RepositoryException.php';
@include_once 'sites/all/libraries/tuque/FedoraRelationships.php';


class FedoraConnection {
	private $FEDORAURL = null;
	private $FEDORAUSER = null;
	private $FEDORAPASS = null;
	public $connection = null;
	public $repository = null;
	public $apim = null;
	public $apia = null;
	
	
	public function __construct($url, $user, $pass)
	{
		$FEDORAURL = $url;
		$FEDORAUSER = $user;
		$FEDORAPASS = $pass;
		
		echo 'in __construct().' . PHP_EOL;
        $this->connection = new RepositoryConnection($FEDORAURL, $FEDORAUSER, $FEDORAPASS);
		$this->api = new FedoraApi($this->connection);
		$this->serializer = new FedoraApiSerializer();
		$this->cache = new SimpleCache();
		$this->repository = new FedoraRepository($this->api, $this->cache);
		
		$this->apim = new FedoraApiM($this->connection, $this->serializer);
		$this->apia = new FedoraApiA($this->connection, $this->serializer);		
	}
	
	public function describeRepository()
	{	
		$describe = $this->apia->describeRepository();
		var_dump($describe);
		// To-do: print out the repository's information
	}
	
	public function getCount($field, $value)  {
		$results = $this->apia->findObjects('query', $field . "=" . $value, PHP_INT_MAX, array('pid'));
		return count($results);
	}
	
	public function findObjects($namespace, $field, $keyword, $quantity, $return) {
		echo "namespace: " . $namespace . ", field: " . $field . ", keyword: " . $keyword . ", quantity: " . $quantity . ", return: " . implode(' ' , $return) . "\n";
		
		$result = null;
		$finalResult = array();
		
		if ((is_null($field)) && (is_null($keyword))) {
			echo "field and keyword are null.\n";
			echo "pid~{$namespace}:*\n";
			$result = $this->apia->findObjects('query', "pid~{$namespace}:*", $quantity, $return);						
		}
		else if (strcmp($field, "pid") == 0) {
			echo "field is PID.\n";
			echo "pid~{$namespace}:{$keyword}*\n";
			$result = $this->apia->findObjects('query', "pid~{$namespace}:{$keyword}*", $quantity, $return);
		}
		else {
			echo "\n\n{$field}~{$keyword}*\n\n";
			$result = $this->apia->findObjects('query', "{$field}~{$keyword}*", $quantity, $return);			
		}
		$finalResult = array_merge_recursive($finalResult, $result);
		
		// if ($quantity > 100) {
		if ($quantity > count($finalResult['results'])) { 
			//echo "quantity greater than 100.\n";
			// $quantity =- count($finalResult['results']);
			
			echo "session: " . $result['session'] . "\n";
			echo "token: " . $result['session']['token'] . "\n";
			if ((array_key_exists("session", $result)) && (array_key_exists("token", $result['session']))) {					
				$token = $result['session']['token'];
				while (!is_null($token)) { 
			 		// echo "in the loop.\n";
					$result = $this->apia->resumeFindObjects($token);
					foreach($result['results'] as $results) {
						// echo "small result count: " . count($result['results']) . "\n";
						if (count($finalResult['results']) < $quantity) {
							// echo "pushing result onto finalResult.\n";
							// var_dump($results);
							//echo "result count antes: " . count($finalResult['results']) . "\n";
							// $finalResult['results'] = array_merge($finalResult['results'], $results);
							array_push($finalResult['results'], $results);
							//echo "result count despues: " . count($finalResult['results']) . "\n";
						}
					}
					if (count($finalResult['results']) == $quantity) {
						break;
					}
					// $finalResult = array_merge($finalResult, $result);
					$token = $result['session']['token'];	
					//echo "token: " . $token . "\n";
					//echo "result count: " . count($finalResult['results']) . "\n";
				}
			}
		}
		
		echo "final result count: " . count($finalResult['results']) . "\n";
		//die();
		return $finalResult;
	}	
	
}

