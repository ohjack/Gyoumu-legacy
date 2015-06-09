<?PHP
/**
 * example that adds information to the item description
 *
 * $Id: example_EndItem.php,v 1.2 2004/11/07 12:48:52 schst Exp $
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

$item   = $ebay->GetItem(4501336808);
$result = $item->End(1);

echo '<pre>';
print_r($result);
echo '</pre>';
?>