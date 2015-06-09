<?PHP
/**
 * example that fetches all transactions for an item
 *
 * $Id: example_GetNotificationPreferences.php,v 1.1 2004/10/28 17:14:53 schst Exp $
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

$prefs = $ebay->GetNotificationPreferences('Application');

echo '<pre>';
print_r($prefs);
echo '</pre>';
?>