using System;
using Avalonia.Controls;
using Avalonia.Input;
using composer.ViewModels;

namespace composer.Views;

public partial class MainView : Window
{
    public MainView()
    {
        InitializeComponent();
        DataContext ??= new MainViewModel();
    }

    protected override void OnOpened(EventArgs e)
    {
        base.OnOpened(e);
        if (DataContext is not MainViewModel vm) return;
        
        vm.StorageProvider = StorageProvider;
            
        UpdateSidebarState(vm);
        PropertyChanged += (s, a) => {
            if (a.Property == WindowStateProperty) UpdateSidebarState(vm);
        };
    }

    private void UpdateSidebarState(MainViewModel vm)
    {
        var isBig = WindowState == WindowState.FullScreen || WindowState == WindowState.Maximized;
        // User requested manual toggle mostly, but this auto-expands on maximize
        // You can remove this line if you want strictly manual control
        vm.Expanded = isBig;
    }

    private void InputElement_OnPointerPressed__SvgImage__Assets_Images_logo_svg_(object? sender, PointerPressedEventArgs e)
    {
        // Double click logo to resize sidebar
        if (e.ClickCount != 2) return;
        (DataContext as MainViewModel)?.SideMenuResizeCommand.Execute(null);
    }
}