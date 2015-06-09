<?PHP
/**
 * example that shows how a call object is 
 * able to show its list of parameters
 *
 * $Id: example_features_DescribingCalls.php,v 1.2 2004/12/23 15:21:06 schst Exp $
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

$call = Services_Ebay::loadAPICall('AddDispute');
echo '<pre>';
$call->describeCall();
echo '</pre>';
?>