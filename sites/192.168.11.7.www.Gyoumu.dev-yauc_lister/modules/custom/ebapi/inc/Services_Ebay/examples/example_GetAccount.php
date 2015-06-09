<?PHP
/**
 * example that searches for items
 *
 * $Id: example_GetAccount.php,v 1.1 2004/10/28 17:14:53 schst Exp $
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

$account = $ebay->GetAccount(Services_Ebay::ACCOUNT_TYPE_PERIOD, 3);

echo '<pre>';
print_r($account->toArray());
echo '</pre>';

foreach ($account as $entry) {
	echo '<pre>';
	print_r($entry);
	echo '</pre>';
}
?>