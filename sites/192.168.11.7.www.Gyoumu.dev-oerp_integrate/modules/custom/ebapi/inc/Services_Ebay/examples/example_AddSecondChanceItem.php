<?PHP
/**
 * example that adds a second chance for a user
 *
 * $Id: example_AddSecondChanceItem.php,v 1.1 2004/10/28 17:14:53 schst Exp $
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

$item = $ebay->AddSecondChanceItem('4501333765', 'loislane-74', 3);

echo '<pre>';
print_r($item->toArray());
echo '</pre>';
?>