<?PHP
/**
 * example that changes the adress of the user
 *
 * $Id: example_SetSellerPaymentAddress.php,v 1.1 2004/10/28 17:14:53 schst Exp $
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

$result = $ebay->SetSellerPaymentAddress('Clark Kent', 'Foo Bar street', null, 'San Francisco', 'CA', 'USA', '94101');

if ($result === true) {
	echo 'Address succesfully updated';
} else {
	echo 'An error occurred.';
}
?>