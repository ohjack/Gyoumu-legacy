<?PHP
/**
 * Example that lists all API calls the are supported by Services_Ebay
 *
 * $Id: example_features_introspection.php,v 1.1 2004/11/30 20:03:48 schst Exp $
 *
 * @package     Services_Ebay
 * @subpackage  Examples
 * @author      Stephan Schmidt <schst@php.net>
 */
error_reporting(E_ALL);
require_once '../Ebay.php';
require_once 'config.php';

$session = Services_Ebay::getSession($devId, $appId, $certId);
$session->setToken($token);

$ebay  = new Services_Ebay($session);
$calls = $ebay->getAvailableApiCalls();

echo '<pre>';
print_r($calls);
echo '</pre>';
?>