<?PHP
/**
 * example that fetches an item
 *
 * $Id: example_GetItemShipping.php,v 1.1 2004/10/28 17:14:53 schst Exp $
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

$shipping = $ebay->GetItemShipping(4501296414, 94101);

echo	"<pre>";
print_r($shipping);
echo	"</pre>";
?>