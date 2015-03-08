<?php

print_r(cls_info('php_server'));

print_r(get_class_vars('php_server'));

function cls_info($clsname, $methods=null){
	$reflection = new ReflectionClass($clsname);
	$aMethods = $reflection->getMethods();
	foreach($aMethods AS $param){
		$name = $param->name;
		$args = array();
		if($methods){
			if(strtolower($name) !== strtolower($methods))
				continue;
		}
		foreach ($param->getParameters() as $param) {
			$tmparg = '';
			if ($param->isPassedByReference()) $tmparg = '&';
			 
			if ($param->isOptional()) {
				try {
					$DefaultValue = $param->getDefaultValue();
					if(is_null($DefaultValue))
						$DefaultValue = 'null';
					 
					$tmparg = '[' . $tmparg . '$' . $param->getName() . ' = ' . $DefaultValue . ']';
				} catch( Exception $e ) {
					$tmparg = '['.$tmparg . '$' . $param->getName().']';
				}
			} else {
				$tmparg .= '$'.$param->getName();
			}
			$args[] = $tmparg;
			unset ($tmparg);
		}
		$functions_list[$name] = 'function ' . $name. '(' . strtr(implode(', ', $args),array('], ['=>', ')) . ')' . PHP_EOL;
	}
	return $functions_list;
}
?>