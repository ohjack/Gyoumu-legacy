<?PHP
/**
 * example that fetches information about a site
 *
 * $Id: example_GeteBayDetails.php,v 1.2 2004/11/30 20:17:25 schst Exp $
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


$details = $ebay->GeteBayDetails();
echo '<pre>';
print_r($details);
echo '</pre>';


// get the shipping services for Germany
$session->setSiteId(Services_Ebay::SITEID_DE);

$details = $ebay->GeteBayDetails('ShippingService');
echo '<pre>';
print_r($details);
echo '</pre>';

?>