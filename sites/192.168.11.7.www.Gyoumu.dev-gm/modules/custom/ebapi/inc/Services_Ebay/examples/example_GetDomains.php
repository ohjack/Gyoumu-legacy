<?PHP
/**
 * example that fetches domains
 *
 * This is supposed to produce an exception
 * as your application probably is not allowed to make
 * this call.
 *
 * $Id: example_GetDomains.php,v 1.1 2004/10/28 17:14:53 schst Exp $
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

$domains = $ebay->GetDomains();
echo '<pre>';
print_r($domains);
echo '</pre>';
?>