
import * as hookMod from '@angular/router-deprecated/src/lifecycle/lifecycle_annotations';
import * as routerMod from '@angular/router-deprecated/router';
import {isBlank, isPresent} from '@angular/common/src/facade/lang';
import {StringMapWrapper} from '@angular/common/src/facade/collection';
import {PromiseWrapper} from '@angular/router-deprecated/src/facade/promise';
//import {PromiseWrapper} from '@angular/common/src/facade/async';
import {BaseException} from '@angular/core/index';
import {ViewContainerRef, DynamicComponentLoader, Directive, Injector, ReflectiveInjector, provide, ComponentRef, Attribute} from '@angular/core';
import {
    ComponentInstruction, CanReuse, OnReuse, CanDeactivate,
    RouterOutlet, OnActivate, Router, RouteData, RouteParams, OnDeactivate
} from '@angular/router-deprecated';
import {hasLifecycleHook} from '@angular/router-deprecated/src/lifecycle/route_lifecycle_reflector';

/**
 * Reference Cache Entry
 */
class RefCacheItem {
    constructor(public componentRef: ComponentRef<any>) {

    }
}

/**
 * Reference Cache
 */
class RefCache {
    private cache: any = {};

    public getRef(type: any) {
        return this.cache[type];
    }

    public addRef(type: any, ref: RefCacheItem) {
        this.cache[type] = ref;
    }

    public hasRef(type: any): boolean {
        return !isBlank(this.cache[type]);
    }

    public removeRef(type: any) {
        delete this.cache[type];
    }

    public remove(ref) {
        // remove self from cache
        this.removeRef(ref.componentType);

        // remove children
        var parent = ref.location.nativeElement;
        Object.keys(this.cache).forEach((key) => {
            var child = this.cache[key].componentRef.location.nativeElement;
            if (parent.contains(child)) {
                this.removeRef(key)
            }
        })

        // will destroy component and all children
        // sense this is an angular call, all lifecycle methods will be called
        ref.dispose();
    }
}

const refCache = new RefCache();
/**
 * An outlet that persists the child views and re-uses their components.
 *
 * @author Wael Jammal
 */
@Directive({ selector: 'persistent-router-outlet' })
export class PersistentRouterOutlet extends RouterOutlet {
    private currentInstruction: ComponentInstruction;
    private currentElementRef;
    private resolveToTrue = PromiseWrapper.resolve(true);
    private currentComponentRef: ComponentRef<any>;

    constructor(elementRef: ViewContainerRef,
        private loader: DynamicComponentLoader,
        private parentRouter: Router,
        @Attribute('name') nameAttr: string) {
        super(elementRef, loader, parentRouter, nameAttr);

        this.currentElementRef = elementRef;
    }

    /**
     * Called by the Router to instantiate a new component during the commit phase of a navigation.
     * This method in turn is responsible for calling the `routerOnActivate` hook of its child.
     */
    public activate(nextInstruction: ComponentInstruction): Promise<any> {
        let previousInstruction = this.currentInstruction;

        this.currentInstruction = nextInstruction;

        if (!refCache.hasRef(nextInstruction.componentType)) {
            let componentType = nextInstruction.componentType;
            let childRouter = this.parentRouter.childRouter(componentType);

            let providers = ReflectiveInjector.resolve([
                provide(RouteData, { useValue: nextInstruction.routeData }),
                provide(RouteParams, { useValue: new RouteParams(nextInstruction.params) }),
                provide(routerMod.Router, { useValue: childRouter })
            ]);

            return this.loader.loadNextToLocation(componentType, this.currentElementRef, providers)
                .then((componentRef) => {
                    refCache.addRef(nextInstruction.componentType, new RefCacheItem(componentRef));

                    this.currentComponentRef = componentRef;

                    if (hasLifecycleHook(hookMod.routerOnActivate, componentType)) {
                        return (<OnActivate>componentRef.instance)
                            .routerOnActivate(nextInstruction, previousInstruction);
                    }
                });
        }
        else {
            let ref = refCache.getRef(nextInstruction.componentType);
            ref.componentRef.location.nativeElement.style.display = 'initial';

            this.currentComponentRef = ref.componentRef;

            return PromiseWrapper.resolve(
                hasLifecycleHook(hookMod.routerOnReuse, this.currentInstruction.componentType) ?
                    (<OnReuse>ref.componentRef.instance).routerOnReuse(nextInstruction, previousInstruction) : true
            );
        }
    }

    /**
     * Called by the Router during the commit phase of a navigation when an outlet
     * reuses a component between different routes.
     * This method in turn is responsible for calling the `routerOnReuse` hook of its child.
     */
    public reuse(nextInstruction: ComponentInstruction): Promise<any> {
        let previousInstruction = this.currentInstruction;
        this.currentInstruction = nextInstruction;

        if (isBlank(this.currentComponentRef)) {
            throw new BaseException(`Cannot reuse an outlet that does not contain a component.`);
        }

        let ref = refCache.getRef(nextInstruction.componentType);
        let currentRef = ref ? ref.componentRef : null;

        return PromiseWrapper.resolve(
            hasLifecycleHook(hookMod.routerOnReuse, this.currentInstruction.componentType) ?
                (<OnReuse>currentRef.instance).routerOnReuse(nextInstruction, previousInstruction) : true
        );
    }

    /**
     * Called by the Router when an outlet disposes of a component's contents.
     * This method in turn is responsible for calling the `routerOnDeactivate` hook of its child.
     */
    public deactivate(nextInstruction: ComponentInstruction): Promise<any> {
        let next = this.resolveToTrue;
        let ref = this.currentComponentRef;

        if (isPresent(ref) && isPresent(this.currentInstruction) &&
            hasLifecycleHook(hookMod.routerOnDeactivate, this.currentInstruction.componentType)) {
            next = PromiseWrapper.resolve(
                (<OnDeactivate>ref.instance)
                    .routerOnDeactivate(nextInstruction, this.currentInstruction));
        }

        return next.then(() => {
            if (isPresent(ref)) {
                if (ref.instance.routerCanReuse && ref.instance.routerCanReuse(nextInstruction, ref.componentType)) {
                    ref.location.nativeElement.style.display = 'none';
                } else {
                    refCache.remove(ref);
                }
            }
        });
    }

    /**
     * Called by the Router during recognition phase of a navigation.
     *
     * If this resolves to `false`, the given navigation is cancelled.
     *
     * This method delegates to the child component's `routerCanDeactivate` hook if it exists,
     * and otherwise resolves to true.
     */
    public routerCanDeactivate(nextInstruction: ComponentInstruction): Promise<boolean> {
        if (isBlank(this.currentInstruction)) {
            return this.resolveToTrue;
        }

        let ref = this.currentComponentRef;

        if (!ref) {
            let foundRef = refCache.getRef(this.currentInstruction.componentType);
            ref = foundRef ? foundRef.componentRef : null;
        }

        if (hasLifecycleHook(hookMod.routerCanDeactivate, this.currentInstruction.componentType)) {
            return PromiseWrapper.resolve(
                (<CanDeactivate>ref.instance)
                    .routerCanDeactivate(nextInstruction, this.currentInstruction));
        }

        return this.resolveToTrue;
    }

    /**
     * Called by the Router during recognition phase of a navigation.
     *
     * If the new child component has a different Type than the existing child component,
     * this will resolve to `false`. You can't reuse an old component when the new component
     * is of a different Type.
     *
     * Otherwise, this method delegates to the child component's `routerCanReuse` hook if it exists,
     * or resolves to true if the hook is not present.
     */
    public routerCanReuse(nextInstruction: ComponentInstruction): Promise<boolean> {
        let result;

        let ref = this.currentComponentRef;

        if (!ref) {
            let foundRef = refCache.getRef(nextInstruction.componentType);
            ref = foundRef ? foundRef.componentRef : null;
        }

        if (isBlank(this.currentInstruction) ||
            this.currentInstruction.componentType !== nextInstruction.componentType) {
            result = false;
        } else if (hasLifecycleHook(hookMod.routerCanReuse, this.currentInstruction.componentType)) {
            result = (<CanReuse>ref.instance)
                .routerCanReuse(nextInstruction, this.currentInstruction);
        } else {
            result = nextInstruction === this.currentInstruction ||
                (isPresent(nextInstruction.params) && isPresent(this.currentInstruction.params) &&
                    StringMapWrapper.equals(nextInstruction.params, this.currentInstruction.params));
        }

        return PromiseWrapper.resolve(result);
    }
}