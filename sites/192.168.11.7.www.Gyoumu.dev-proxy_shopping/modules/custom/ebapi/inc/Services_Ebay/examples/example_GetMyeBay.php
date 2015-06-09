<?PHP
/**
 * example that display information in My eBay
 *
 * $Id: example_GetMyeBay.php,v 1.1 2004/10/28 17:14:53 schst Exp $
 *
 * @package     Services_Ebay
 * @subpackage  Examples
 * @author      Stephan Schmidt
 */
error_reporting(E_ALL);
require_once '../Ebay.php';
require_once 'config.php';

$session = Services_Ebay::getSession($devId, $appId, $certId);

$session->setToken($token);

$ebay = new Services_Ebay($session);

$MyeBay = $ebay->GetMyeBay();

foreach ($MyeBay as $listname => $list) {
	echo '<b>'.$listname.'</b><br />';
	foreach ($list as $item) {
		echo $item;
		echo '<br />';
	}
}

?>