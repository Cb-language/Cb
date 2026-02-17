using System;
using System.ComponentModel;
using Avalonia.Controls;
using Avalonia.Input;
using Avalonia.VisualTree;
using composer.ViewModels;

namespace composer.Views;

public partial class MainView : Window
{
    private FileTabViewModel? _lastTab;
    
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
        vm.PropertyChanged += OnVmPropertyChanged;
        
        UpdateSidebarState(vm);
        // ReSharper disable once UnusedParameter.Local
        PropertyChanged += (s, a) => {
            if (a.Property == WindowStateProperty) UpdateSidebarState(vm);
        };
    }

    private void OnVmPropertyChanged(object? sender, PropertyChangedEventArgs e)
    {
        if (e.PropertyName != nameof(MainViewModel.SelectedTab)) return;
        if (sender is not MainViewModel vm) return;
        
        if (_lastTab is not null)
            _lastTab.PropertyChanged -= OnTabPropertyChanged;
            
        if(vm.SelectedTab is null)
            return;

        vm.SelectedTab.PropertyChanged += OnTabPropertyChanged;
        _lastTab = vm.SelectedTab;
    }

    private void OnTabPropertyChanged(object? sender, PropertyChangedEventArgs e)
    {
        if (e.PropertyName != nameof(FileTabViewModel.DiagnosticsJson)) return;
        if (sender is not FileTabViewModel tab) return;

        // Find the TabControl
        var tabControl = this.FindControl<TabControl>("TabControl");
        if (tabControl is null) return;
        
        var tabItem = tabControl.ContainerFromItem(tab) as TabItem;
        
        if (tabItem is null) return;

        // Find the CodeEditor within this TabItem
        var codeEditor = tabItem.FindDescendantOfType<CodeEditor>();
        codeEditor?.HighlightErrors(tab.DiagnosticsJson);
    }

    private void UpdateSidebarState(MainViewModel vm)
    {
        var isBig = WindowState == WindowState.FullScreen || WindowState == WindowState.Maximized;
        vm.Expanded = isBig;
    }

    private void InputElement_OnPointerPressed__SvgImage__Assets_Images_logo_svg_(object? sender, PointerPressedEventArgs e)
    {
        if (e.ClickCount != 2) return;
        (DataContext as MainViewModel)?.SideMenuResizeCommand.Execute(null);
    }
}