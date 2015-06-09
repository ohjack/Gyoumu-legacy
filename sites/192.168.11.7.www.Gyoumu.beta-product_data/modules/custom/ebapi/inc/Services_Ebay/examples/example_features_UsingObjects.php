<?PHP
/**
 * example that shows how to load an API call
 * and use it without the wrapper.
 *
 * $Id: example_features_UsingObjects.php,v 1.1 2004/11/30 20:03:48 schst Exp $
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

$call = Services_Ebay::loadAPICall('GetEbayOfficialTime');
$result = $call->call($session);

echo $result;
?>