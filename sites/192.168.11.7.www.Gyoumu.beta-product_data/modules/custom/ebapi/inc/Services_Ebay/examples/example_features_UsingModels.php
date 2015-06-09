<?PHP
/**
 * example that shows how to load an API call
 * and use it without the wrapper.
 *
 * $Id: example_features_UsingModels.php,v 1.1 2004/11/30 20:03:48 schst Exp $
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

$item = Services_Ebay::loadModel('Item', '4501296414', $session);

echo	"<pre>";
print_r($item->toArray());
echo	"</pre>";

$item->Get();

echo	"<pre>";
print_r($item->toArray());
echo	"</pre>";
?>