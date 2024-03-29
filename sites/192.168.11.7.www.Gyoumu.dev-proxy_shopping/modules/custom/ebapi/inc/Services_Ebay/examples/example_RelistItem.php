<?PHP
/**
 * example that fetches an item
 *
 * $Id: example_RelistItem.php,v 1.2 2004/11/07 12:48:52 schst Exp $
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

$item = $ebay->GetItem(4501333179);
$relist = $item->Relist();

echo	"<pre>";
print_r($relist);
echo	"</pre>";
?>