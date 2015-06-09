<?PHP
/**
 * example that fetches an item
 *
 * $Id: example_GetCrossPromotions.php,v 1.1 2004/10/28 17:14:53 schst Exp $
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

$promo = $ebay->GetCrossPromotions(4501296414);

echo	"<pre>";
print_r($promo);
echo	"</pre>";

$item = $ebay->GetItem(4501296414);

echo	"<pre>";
print_r($item->GetCrossPromotions());
echo	"</pre>";

?>