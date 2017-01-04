import { RouteReuseStrategy, DetachedRouteHandle,ActivatedRouteSnapshot } from '@angular/router';
//https://www.softwarearchitekt.at/post/2016/12/02/sticky-routes-in-angular-2-3-with-routereusestrategy.aspx
// This impl. bases upon one that can be found in the router's test cases.
export class CustomReuseStrategy implements RouteReuseStrategy {

    handlers: {[key: string]: DetachedRouteHandle} = {};

    shouldDetach(route: ActivatedRouteSnapshot): boolean {
        console.debug('CustomReuseStrategy:shouldDetach', route);
        return true;
    }

    store(route: ActivatedRouteSnapshot, handle: DetachedRouteHandle): void {
        console.debug('CustomReuseStrategy:store', route, handle);
        this.handlers[route.routeConfig.path] = handle;
    }

    shouldAttach(route: ActivatedRouteSnapshot): boolean {
        console.debug('CustomReuseStrategy:shouldAttach', route);
        return !!route.routeConfig && !!this.handlers[route.routeConfig.path];
    }

    retrieve(route: ActivatedRouteSnapshot): DetachedRouteHandle {
        console.debug('CustomReuseStrategy:retrieve', route);
        if (!route.routeConfig) return null;
        return this.handlers[route.routeConfig.path];
    }

    shouldReuseRoute(future: ActivatedRouteSnapshot, curr: ActivatedRouteSnapshot): boolean {
        console.debug('CustomReuseStrategy:shouldReuseRoute', future, curr);
        return future.routeConfig === curr.routeConfig;
    }

}