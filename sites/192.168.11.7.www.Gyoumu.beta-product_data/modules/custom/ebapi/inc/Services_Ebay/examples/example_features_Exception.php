<?PHP
/**
 * example that makes use of the Exception handling
 *
 * $Id: example_features_Exception.php,v 1.1 2004/11/30 20:03:48 schst Exp $
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

try {
    $foo = $ebay->UnknownMethod('foo');
} catch (Services_Ebay_Exception $e) {
    echo 'An error occured: '.$e->getMessage();
}
?>